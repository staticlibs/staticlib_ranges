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
 * File:   filter.hpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:34 PM
 */

#ifndef STATICLIB_RANGES_FILTER_HPP
#define STATICLIB_RANGES_FILTER_HPP

#include <array>
#include <iterator>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

#include "staticlib/ranges/refwrap.hpp"
#include "staticlib/ranges/traits.hpp"

namespace staticlib {
namespace ranges {

namespace detail_filter {

/**
 * Lazy `InputIterator` implementation for `filter`  operation.
 * Does not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Moves element from source iterator, checks it against specified `Predicate`
 * and on success moves it out from `operator*` method.
 * Elements, that do not match predicate will be applied to specified `FunctionObject`.
 */
template <typename Iter, typename Elem, typename Pred, typename Dest>
class filtered_iter {
    Iter source_iter;
    Iter source_iter_end;
    // non-owning pointers
    Pred* predicate;
    Dest* offcast_dest;
    // space in iter for placement of Elem instance (to not require DefaultConstructible)
    typename std::aligned_storage<sizeof(Elem), std::alignment_of<Elem>::value>::type current_space;
    Elem* current_ptr = nullptr;
    
public:
    using value_type = Elem;
    // does not support input_iterator, but valid tag is required
    // for std::iterator_traits with libc++ on mac
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::nullptr_t;
    using pointer = std::nullptr_t;
    using reference = std::nullptr_t;

    /**
     * Constructor
     * 
     * @param source_iter source `begin` iterator
     * @param source_iter_end source `past_the_end` iterator
     * @param predicate filtering `Predicate`
     * @param offcast_dest `FunctionObject` for offcast elements
     */
    filtered_iter(Iter source_iter, Iter source_iter_end, Pred& predicate, Dest& offcast_dest) :
    source_iter(std::move(source_iter)),
    source_iter_end(std::move(source_iter_end)),
    predicate(&predicate),
    offcast_dest(&offcast_dest) {
        if (this->source_iter != this->source_iter_end) {
            this->current_ptr = new (std::addressof(current_space)) Elem(std::move(*this->source_iter));
            if (!(*this->predicate)(*current_ptr)) {
                (*this->offcast_dest)(std::move(*current_ptr));
                next();
            }
        }
    }
    
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    filtered_iter(const filtered_iter& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    filtered_iter& operator=(const filtered_iter& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    filtered_iter(filtered_iter&& other) :
    source_iter(std::move(other.source_iter)),
    source_iter_end(std::move(other.source_iter_end)),
    predicate(std::move(other.predicate)),
    offcast_dest(std::move(other.offcast_dest)) { 
        if (other.current_ptr) {
            this->current_ptr = new (std::addressof(current_space)) Elem(std::move(*other.current_ptr));
        }
    }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    filtered_iter& operator=(filtered_iter&& other) {
        this->source_iter = std::move(other.source_iter);
        this->source_iter_end = std::move(other.source_iter_end);
        this->predicate = std::move(other.predicate);
        this->offcast_dest = std::move(other.offcast_dest);
        if (other.current_ptr) {
            *this->current_ptr = std::move(*other.current_ptr);
        }
        return *this;
    }
    
    /**
     * Destructor to clean-up current object
     */
    ~filtered_iter() {
        if (current_ptr) {
            current_ptr->~Elem();
        }
    }

    /**
     * Will iterate over source elements until successful `Predicate`
     * application or source exhaustion.
     * Offcast elements will be applied to `FunctionObject`
     * 
     * @return reference to this iterator
     */
    filtered_iter& operator++() {
        next();
        return *this;
    }

    /**
     * Will iterate over source elements until successful `Predicate`
     * application or source exhaustion.
     * Offcast elements will be applied to `FunctionObject
     * 
     * @return reference to this iterator
     */
    filtered_iter& operator++(int) {
        next();
        return *this;
    }
    
    /**
     * Will move out current element that matched predicate
     * 
     * @return current element
     */
    Elem operator*() {
        return std::move(*current_ptr);
    }

    /**
     * Delegated operator implementation, does NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end "past the end" iterator
     * @return whether not both this and specified iterators are "past the end"
     */    
    bool operator!=(const filtered_iter& end) const {
        return this->source_iter != end.source_iter;
    }

private:
    void next() {
        for (++source_iter; source_iter != source_iter_end; ++source_iter) {
            *current_ptr = std::move(*source_iter);
            auto& ref = *current_ptr;
            if ((*predicate)(ref)) break;
            (*offcast_dest)(std::move(*current_ptr));
        }
    }
    
};

/**
 * Helper template to be used with
 * reference input range when filtered out elements
 * can be safely discarded
 */
template<typename T>
class offcaster {
public:
    void operator()(const T&) {
        // ignore
    }

    void operator()(T&&) {
        // ignore
    }
};

} // namespace


/**
 * Lazy implementation of `SinglePassRange` for `filter`  operation, 
 * after the pass all accessed elements of source range will be moved from
 * (will retain in "valid but unspecified" state). Elements that won't match the 
 * `Predicate` will be applied to specified `FunctionObject`
 */
template <typename Range, typename Pred, typename Dest>
class filtered_range {
    Range source_range;
    Pred predicate;
    Dest offcast_dest;

public:
    /**
     * Type of iterator of this range
     */
    using iterator = decltype(std::declval<decltype(source_range)>().begin());

    /**
     * Result value type of iterators returned from this range
     */
    using value_type = typename std::iterator_traits<iterator>::value_type;

    /**
     * Constructor,
     * created range wrapper will own specified ranges
     * 
     * @param source_range reference to source range
     * @param predicate `Predicate` to check source element againt it
     * @param offcast_dest `FunctionObject` to apply offcast elements to it
     */
    filtered_range(Range&& source_range, Pred predicate, Dest offcast_dest) :
    source_range(std::move(source_range)),
    predicate(std::move(predicate)),
    offcast_dest(std::move(offcast_dest)) { }
    
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    filtered_range(const filtered_range& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    filtered_range& operator=(const filtered_range& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    filtered_range(filtered_range&& other) :
    source_range(std::move(other.source_range)), 
    predicate(std::move(other.predicate)),
    offcast_dest(std::move(other.offcast_dest)) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    filtered_range& operator=(filtered_range&& other) = delete;

    /**
     * Returns `begin` filtered iterator
     * 
     * @return `begin` iterator
     */
    detail_filter::filtered_iter<iterator, value_type, Pred, Dest> begin() {
        return detail_filter::filtered_iter<iterator, value_type, Pred, Dest>{
            std::move(source_range.begin()), std::move(source_range.end()), predicate, offcast_dest
        };
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    detail_filter::filtered_iter<iterator, value_type, Pred, Dest> end() {
        return detail_filter::filtered_iter<iterator, value_type, Pred, Dest>{
            std::move(source_range.end()), std::move(source_range.end()), predicate, offcast_dest
        };
    }

    /**
     * Process this range eagerly returning results as 
     * a newly-allocated vector.
     * 
     * @return vector with processed elements
     */
    std::vector<value_type> to_vector() {
        std::vector<value_type> vec;
        for (auto&& el : *this) {
            vec.emplace_back(std::move(el));
        }
        return vec;
    }
};


/**
 * Lazily filters input range into output range checking each element using
 * specified `Predicate`. Elements are moved from source range one by one,
 * All accessed elements of source range will be left in "valid but unspecified state".
 * Elements that won't match the `Predicate` will be applied to specified `FunctionObject`.
 * Created range wrapper will own specified ranges
 * 
 * @param source_range reference to source range
 * @param predicate `Predicate` to check source element against it
 * @param offcast_dest `FunctionObject` to apply offcast elements to it
 * @return filtered range
 */
template <typename Range, typename Pred, typename Dest,
        class = typename std::enable_if<!std::is_lvalue_reference<Range>::value>::type>
filtered_range<Range, Pred, Dest>
filter(Range&& source_range, Pred predicate, Dest offcast_dest) {
    return filtered_range<Range, Pred, Dest>(
            std::move(source_range), std::move(predicate), std::move(offcast_dest));
}

/**
 * Lazily filters input range into output range checking each element using
 * specified `Predicate`. Elements are moved from source range one by one,
 * All accessed elements of source range will be left in "valid but unspecified state".
 * Created range wrapper will own specified range.
 * 
 * @param source_range reference to source range
 * @param predicate `Predicate` to check source element against it
 * @return filtered range
 */
template <typename Range, typename Pred,
class = typename std::enable_if<!std::is_lvalue_reference<Range>::value>::type>
auto filter(Range&& source_range, Pred predicate) ->
        filtered_range<Range, Pred,
            detail_filter::offcaster<
                typename std::iterator_traits<
                    decltype(std::declval<decltype(source_range)>().begin())
                >::value_type
            >
        > {
    return filter(std::move(source_range), std::move(predicate),
            detail_filter::offcaster<
                typename std::iterator_traits<
                    decltype(std::declval<decltype(source_range)>().begin())
                >::value_type
            >());
}

/**
 * Lazily filters input range into output range checking each element using
 * specified `Predicate`. Elements are moved from source range one by one,
 * All accessed elements of source range will be left in "valid but unspecified state".
 * Created range wrapper will own specified range.
 * This overload is a "special-case" that will accept only (expectedly "temporary") input
 * ranges which contain `std::reference_wrapper` elements.
 * 
 * @param source_range reference to source range
 * @param predicate `Predicate` to check source element against it
 * @return filtered range
 */
template <typename Range, typename Pred,
        class = typename std::enable_if<is_reference_wrapper<typename Range::value_type>::value>::type>
auto filter(Range& source_range, Pred predicate) ->
        filtered_range<Range, Pred,
            detail_filter::offcaster<
                typename std::iterator_traits<
                    decltype(std::declval<decltype(source_range)>().begin())
                >::value_type
            >
        > {
    return filter(std::move(source_range), std::move(predicate),
            detail_filter::offcaster <
                typename std::iterator_traits <
                    decltype(std::declval<decltype(source_range)>().begin())
                >::value_type
            > ());
}

/**
 * Lazily filters input range into output range checking each element using
 * specified `Predicate`. Elements are taken by reference.
 * Created range wrapper will NOT own specified range.
 * 
 * @param source_range reference to source range
 * @param predicate `Predicate` to check source element against it
 * @param offcast_dest `FunctionObject` to apply offcast elements to it
 * @return filtered range
 */
template <typename Range, typename Pred,
        class = typename std::enable_if<!is_reference_wrapper<typename Range::value_type>::value>::type>
auto filter(Range& source_range, Pred predicate) -> 
        filtered_range<staticlib::ranges::refwrapped_range<Range>, Pred,
            detail_filter::offcaster<
                std::reference_wrapper<
                    typename std::iterator_traits<
                        decltype(std::declval<decltype(source_range)>().begin())
                    >::value_type
                >
            >
        > {
    return filter(staticlib::ranges::refwrap(source_range), std::move(predicate),
            detail_filter::offcaster<
                std::reference_wrapper<
                    typename std::iterator_traits<
                        decltype(std::declval<decltype(source_range)>().begin())
                    >::value_type
                >
            >());
}

/**
 * Lazily filters input range into output range checking each element using
 * specified `Predicate`. Elements are taken by reference.
 * Created range wrapper will NOT own specified range.
 * 
 * @param source_range reference to source range
 * @param predicate `Predicate` to check source element against it
 * @param offcast_dest `FunctionObject` to apply offcast elements to it
 * @return filtered range
 */
template <typename Range, typename Pred>
auto filter(const Range& source_range, Pred predicate) ->
        filtered_range<staticlib::ranges::refwrapped_const_range<Range>, Pred,
            detail_filter::offcaster<
                std::reference_wrapper<
                    const typename std::iterator_traits<
                        decltype(std::declval<decltype(source_range)>().begin())
                    >::value_type
                >
            >
        > {
    return filter(staticlib::ranges::refwrap(source_range), std::move(predicate),
        detail_filter::offcaster<
            std::reference_wrapper<
                const typename std::iterator_traits <
                    decltype(std::declval<decltype(source_range)>().begin())
                >::value_type
            >
        >());
}

} // namespace
}

#endif    /* STATICLIB_RANGES_FILTER_HPP */

