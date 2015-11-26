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
#define	STATICLIB_RANGES_RANGE_ADAPTER_HPP

#include <array>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace staticlib {
namespace ranges {

namespace detail {

template <typename R>
class range_adapter_iter {
    R& range;
    bool past_the_end = false;
    
public:    
    typedef typename R::value_type value_type;
    // does not support input_iterator, but valid tag is required
    // for std::iterator_traits with libc++ on mac
    typedef std::input_iterator_tag iterator_category;
    typedef std::nullptr_t difference_type;
    typedef std::nullptr_t pointer;
    typedef std::nullptr_t reference;

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
    range_adapter_iter(range_adapter_iter&& other) :
    range(other.range),
    past_the_end(other.past_the_end) { }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    range_adapter_iter& operator=(range_adapter_iter&& other) = delete;
    
    range_adapter_iter(R& range) :
    range(range) { }

    range_adapter_iter(R& range, bool past_the_end) :
    range(range), past_the_end(past_the_end) { }    
    
    range_adapter_iter& operator++() {
        past_the_end = !range.compute_next_and_set_state();
        return *this;
    }

    range_adapter_iter& operator++(int) {
        past_the_end = !range.compute_next_and_set_state();
        return *this;
    }

    typename R::value_type operator*() {
        return std::move(range.get_current());
    }

    bool operator!=(const range_adapter_iter& end) const {
        return this->past_the_end != end.past_the_end;
    }
    
};

} // namespace

template <typename R, typename E>
class range_adapter {
    enum class State { CREATED, READY, CONSUMED, EXHAUSTED };
    friend class detail::range_adapter_iter<R>;

    // space in iter for placement of E instance (to not require DefaultConstructible)
    std::array<char, sizeof(E)> current_space;
    E* current_ptr;
    
    State state = State::CREATED;    
    
public:
    /**
     * Result value type of iterators returned from this range
     */
    typedef E value_type;

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
    current_space(),
    current_ptr(),
    state(other.state) {
        if (other.current_ptr) {
            this->current_ptr = new (current_space.data()) E(std::move(*other.current_ptr));
        }
    }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    range_adapter& operator=(range_adapter&& other) {
        this->state = std::move(other.state);
        if (other.current_ptr) {
            *this->current_ptr = std::move(*other.current_ptr);
        }
        return *this;
    }

    /**
     * Destructor to clean-up current object
     */
    ~range_adapter() {
        if (current_ptr) {
            current_ptr->~E();
        }
    }

    /**
     * Returns `begin` transformed iterator
     * 
     * @return `begin` iterator
     */
    detail::range_adapter_iter<R> begin() {
        if (State::CREATED == state) {
            compute_next_and_set_state();
            return detail::range_adapter_iter<R>(*static_cast<R*>(this));
        } else {
            throw std::range_error("Invalid attempt to get a 'begin()' iterator the second time");
        }
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    detail::range_adapter_iter<R> end() {
        return detail::range_adapter_iter<R>(*static_cast<R*>(this), true);
    }

protected:

    E get_current() {
        switch(state) {
        case State::READY: {
                state = State::CONSUMED;
                return std::move(*current_ptr);
            }
        default:
            throw std::range_error("Invalid attempt to dereference a range that is not ready");
        }
    }

    bool set_current(E&& current) {
        switch (state) {
        case State::CREATED:
            this->current_ptr = new (current_space.data()) E(std::move(current));
            break;
        case State::READY:
        case State::CONSUMED:
            * this->current_ptr = std::move(current);
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
    bool compute_next_and_set_state() {
        switch (state) {
        case State::CREATED:
        case State::READY:
        case State::CONSUMED: {
            bool has_next = static_cast<R*> (this)->compute_next();
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

};

} // namespace
}

#endif	/* STATICLIB_RANGES_RANGE_ADAPTER_HPP */

