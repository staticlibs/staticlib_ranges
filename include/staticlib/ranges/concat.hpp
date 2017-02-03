/*
 * Copyright 2015, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICElemNSElem-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIElemS OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   concat.hpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:33 PM
 */

#ifndef STATICLIB_RANGES_CONCAT_HPP
#define STATICLIB_RANGES_CONCAT_HPP

#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

#include "staticlib/ranges/refwrap.hpp"

namespace staticlib {
namespace ranges {

namespace detail_concat {

/**
 * Lazy `InputIterator` implementation for `concat` (or `chain`) operation.
 * Does not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Moves element from source iterators one by one and moves it out from `operator*` method.
 */
template<typename Iter1, typename Iter2, typename Elem>
class concatted_iter {
    Iter1 source_iter1;
    Iter1 source_iter1_end;
    Iter2 source_iter2;

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
     * @param source_iter1 `begin` first source iterator
     * @param source_iter1_end `past_the_end` first source iterator
     * @param source_iter2 `begin` second iterator
     */
    concatted_iter(Iter1 source_iter1, Iter1 source_iter1_end, Iter2 source_iter2) :
    source_iter1(std::move(source_iter1)),
    source_iter1_end(std::move(source_iter1_end)),
    source_iter2(std::move(source_iter2)) { }
    
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    concatted_iter(const concatted_iter& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    concatted_iter& operator=(const concatted_iter& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    concatted_iter(concatted_iter&& other) :
    source_iter1(std::move(other.source_iter1)),
    source_iter1_end(std::move(other.source_iter1_end)),
    source_iter2(std::move(other.source_iter2)) { }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    concatted_iter& operator=(concatted_iter&& other) {
        this->source_iter1 = std::move(other.source_iter1);
        this->source_iter1_end = std::move(other.source_iter1_end);
        this->source_iter2 = std::move(other.source_iter2);
        return *this;
    }

    /**
     * Increments first source iterator if it is not yet exhausted, 
     * otherwise - increments second source iterator.
     * 
     * @return reference to this iterator
     */
    concatted_iter& operator++() {
        next();
        return *this;
    }

    /**
     * Increments first source iterator if it is not yet exhausted, 
     * otherwise - increments second source iterator.
     * 
     * @return reference to this iterator
     */
    concatted_iter& operator++(int) {
        next();
        return *this;
    }

    /**
     * Will move out current element
     * 
     * @return current element
     */
    Elem operator*() {
        if (source_iter1 != source_iter1_end) {
            return std::move(*source_iter1);
        } else {
            return std::move(*source_iter2);
        }
    }

    /**
     * Delegated operator implementation, does NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end "past the end" iterator
     * @return whether not both this and specified iterators are "past the end"
     */    
    bool operator!=(const concatted_iter& end) const {
        return this->source_iter1 != end.source_iter1 ||
                this->source_iter2 != end.source_iter2;
    }
    
private:    
    void next() {
        if (source_iter1 != source_iter1_end) {
            ++source_iter1;
        } else {
            ++source_iter2;
        }
    }
};

} // namespace


/**
 * Lazy implementation of `SinglePassRange` for `concat` (or `chain`)  operation, 
 * after the pass all accessed elements of source ranges will be moved from
 * (will retain in "valid but unspecified" state).
 */
template <typename Range1, typename Range2>
class concatted_range {
    Range1 source_range1;
    Range2 source_range2;

public:
    /**
     * Type of iterator of first source range
     */
    using iterator = decltype(std::declval<decltype(source_range1)>().begin());
    /**
     * Type of iterator of second source range
     */
    using iterator2 = decltype(std::declval<decltype(source_range2)>().begin());
    /**
     * Result value type of iterators returned from this range
     */
    using value_type = typename std::iterator_traits<iterator>::value_type;

    /**
     * Constructor,
     * created range wrapper will own specified ranges
     * 
     * @param source_range1 first source range
     * @param source_range2 second source range
     */
    concatted_range(Range1&& source_range1, Range2&& source_range2) :
    source_range1(std::move(source_range1)),
    source_range2(std::move(source_range2)) { }
    
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    concatted_range(const concatted_range& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    concatted_range& operator=(const concatted_range& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    concatted_range(concatted_range&& other) :
    source_range1(std::move(other.source_range1)), 
    source_range2(std::move(other.source_range2)) { };

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    concatted_range& operator=(concatted_range&& other) = delete;

    /**
     * Returns `begin` concatenated iterator
     * 
     * @return `begin` iterator
     */    
    detail_concat::concatted_iter<iterator, iterator2, value_type> begin() {
        // move here is required by msvs
        return detail_concat::concatted_iter<iterator, iterator2, value_type>{
            std::move(source_range1.begin()), std::move(source_range1.end()), std::move(source_range2.begin())
        };
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    detail_concat::concatted_iter<iterator, iterator2, value_type> end() {
        return detail_concat::concatted_iter<iterator, iterator2, value_type>{
            std::move(source_range1.end()), std::move(source_range1.end()), std::move(source_range2.end())
        };
    }

    /**
     * Process this range eagerly returning results as a vector
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
 * Lazily concatenates two input ranges into single output range.
 * Elements are moved from source ranges one by one,
 * All accessed elements of source ranges will be left in "valid but unspecified state".
 * Created range wrapper will own specified ranges
 * 
 * @param source_range1 first source range
 * @param source_range2 second source range
 * @return concatenated range
 */
template <typename Range1, typename Range2,
        class = typename std::enable_if<!std::is_lvalue_reference<Range1>::value>::type,
        class = typename std::enable_if<!std::is_lvalue_reference<Range2>::value>::type> 
concatted_range<Range1, Range2>
concat(Range1&& range1, Range2&& range2) {
    return concatted_range<Range1, Range2> (std::move(range1), std::move(range2));
}

/**
 * Lazily concatenates two input ranges into single output range
 * taking elements by reference.
 * 
 * @param source_range1 first source range
 * @param source_range2 second source range
 * @return concatenated range
 */
template <typename Range1, typename Range2>
concatted_range<staticlib::ranges::refwrapped_range<Range1>, staticlib::ranges::refwrapped_range<Range2>> 
concat(Range1& range1, Range2& range2) {
    return concat(staticlib::ranges::refwrap(range1), staticlib::ranges::refwrap(range2));
}

/**
 * Lazily concatenates two input ranges into single output range
 * taking elements by reference.
 * 
 * @param source_range1 first source range
 * @param source_range2 second source range
 * @return concatenated range
 */
template <typename Range1, typename Range2>
concatted_range<staticlib::ranges::refwrapped_const_range<Range1>, staticlib::ranges::refwrapped_const_range<Range2>>
concat(const Range1& range1, const Range2& range2) {
    return concat(staticlib::ranges::refwrap(range1), staticlib::ranges::refwrap(range2));
}

} // namespace
}

#endif    /* STATICLIB_RANGES_CONCAT_HPP */

