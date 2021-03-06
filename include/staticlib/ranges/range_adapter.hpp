/*
 * Copyright 2015, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   range_adapter.hpp
 * Author: alex
 *
 * Created on November 26, 2015, 9:51 AM
 */

#ifndef STATICLIB_RANGES_RANGE_ADAPTER_HPP
#define STATICLIB_RANGES_RANGE_ADAPTER_HPP

#include <array>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace staticlib {
namespace ranges {

namespace detail_adapter {

/**
 * Iterator implementation for the range adapter.
 * Holds a reference to the range and calls it for next elements.
 */
template <typename Range>
class range_adapter_iter : public std::iterator<std::input_iterator_tag, typename Range::value_type> {
    Range* range;
    
public:
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    range_adapter_iter(const range_adapter_iter& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    range_adapter_iter& operator=(const range_adapter_iter& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    range_adapter_iter(range_adapter_iter && other) :
    range(other.range) { 
        other.range = nullptr;
    }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    range_adapter_iter& operator=(range_adapter_iter && other) {
        this->range = other.range;
        other.range = nullptr;
        return *this;
    }
    
    /**
     * Constructor
     * 
     * @param range parent range
     * @param past_the_end true for the "past the end" iterator
     */
    range_adapter_iter(Range* range) :
    range(range) { }    
    
    /**
     * Iterates to the next element
     * 
     * @return reference to this iterator instance
     */
    range_adapter_iter& operator++() {
        if (range) {
            range->compute_next_and_set_state();
        }
        return *this;
    }

    /**
     * Iterates to the next element
     * 
     * @return reference to this iterator instance
     */
    range_adapter_iter& operator++(int) {
        if (range) {
            range->compute_next_and_set_state();
        }
        return *this;
    }

    /**
     * Returns current element
     * 
     * @return current element
     */
    typename Range::value_type operator*() {
        if (range) {
            return std::move(range->get_current());
        } else {
            throw std::range_error("Invalid attempt to dereference a 'past_the_end' iterator");
        }
    }

    /**
     * Compares this iterator instance with a "past the end"
     * Does NOT support arbitrary input instances,
     * should be used only to compare with "past the end" iterator.
     * 
     * @param end "past the end" iterator
     * @return whether not both this and specified iterators are "past the end"
     */
    bool operator!=(const range_adapter_iter& end) const {
        return !(!this->range || Range::State::EXHAUSTED == this->range->state) ||
                !(!end.range || Range::State::EXHAUSTED == end.range->state);
    }
    
};

} // namespace

/**
 * Abstract Range for move-only objects, implements all the boilerplate
 * to support C++11 `for` loops. Inheritors should implement a single method
 * `compute_next` that should set next element as `current` using 
 * `set_current` and return `true`, or return `false` if range is exhausted.
 * Inheritors should use CRTP - `compute_next` will be called using compile-time
 * polymorphism.
 */
template <typename Range, typename Elem>
class range_adapter {
    enum class State { CREATED, READY, CONSUMED, EXHAUSTED };
    friend class detail_adapter::range_adapter_iter<Range>;

    // space in iter for placement of Elem instance (to not require DefaultConstructible)
    typename std::aligned_storage<sizeof(Elem), std::alignment_of<Elem>::value>::type current_space;
    Elem* current_ptr = nullptr;
    
    State state = State::CREATED;    
    
protected:
    /**
     * Constructor for inheritors
     */
    range_adapter() { }

    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    range_adapter(const range_adapter& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    range_adapter& operator=(const range_adapter& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    range_adapter(range_adapter&& other) :
    state(other.state) {
        if (other.current_ptr) {
            this->current_ptr = new (std::addressof(current_space)) Elem(std::move(*other.current_ptr));
            other.state = State::EXHAUSTED;
        }
    }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    range_adapter& operator=(range_adapter&& other) {
        this->state = other.state;
        if (other.current_ptr) {
            *this->current_ptr = std::move(*other.current_ptr);
            other.state = State::EXHAUSTED;
        }
        return *this;
    }

    
public:
    /**
     * Result value type of iterators returned from this range
     */
    using value_type = Elem;
    

    /**
     * Destructor to clean-up current object,
     * non-virtual, as pointers to base class should not be used.
     */
    ~range_adapter() {
        if (current_ptr) {
            current_ptr->~Elem();
        }
    }

    /**
     * Returns `begin` iterator
     * 
     * @return `begin` iterator
     */
    detail_adapter::range_adapter_iter<Range> begin() {
        if (State::CREATED == state) {
            compute_next_and_set_state();
            return detail_adapter::range_adapter_iter<Range>(static_cast<Range*>(this));
        } else {
            throw std::range_error("Invalid attempt to get a 'begin()' iterator the second time");
        }
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    detail_adapter::range_adapter_iter<Range> end() {
        return detail_adapter::range_adapter_iter<Range>(nullptr);
    }

protected:

    /**
     * Sets current element for this range, this element will be move-returned
     * through the next iterator dereference call. 
     * Inheritors should call this method from the `compute_next` method
     * passing the next element.
     * 
     * @param current next element to be returned through the iterator dereference
     * @return true
     */
    bool set_current(Elem&& current) {
        switch (state) {
        case State::CREATED:
            this->current_ptr = new (std::addressof(current_space)) Elem(std::move(current));
            break;
        case State::READY:
        case State::CONSUMED:
            *this->current_ptr = std::move(current);
            break;
        case State::EXHAUSTED:
            throw std::range_error("Invalid attempt to set element for the exhausted range");
        default:
            throw std::runtime_error("Unsupported range state");
        }
        state = State::READY;
        return true;
    }
    
private:
    /**
     * Calls the `compute_next` method and sets the state according to its results.
     * 
     * @return true if next element exists, false (range exhausted) otherwise
     */
    bool compute_next_and_set_state() {
        switch (state) {
        case State::CREATED:
        case State::READY:
        case State::CONSUMED: {
            bool has_next = static_cast<Range*> (this)->compute_next();
            if (!has_next) {
                state = State::EXHAUSTED;
            }
            return has_next;
        }
        case State::EXHAUSTED:
            throw std::range_error("Invalid attempt to increment an exhausted range");
        default:
            throw std::runtime_error("Unsupported range state");
        }
    }

    /**
     * Move-return accessor to the current element of this range
     * 
     * @return curennt element
     */
    Elem get_current() {
        switch (state) {
        case State::READY:
        {
            state = State::CONSUMED;
            return std::move(*current_ptr);
        }
        default:
            throw std::range_error("Invalid attempt to dereference a range that is not ready");
        }
    }

};

} // namespace
}

#endif /* STATICLIB_RANGES_RANGE_ADAPTER_HPP */

