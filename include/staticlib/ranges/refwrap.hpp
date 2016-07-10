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
 * File:   refwrap.hpp
 * Author: alex
 *
 * Created on February 16, 2015, 10:58 AM
 */

#ifndef STATICLIB_RANGES_REFWRAP_HPP
#define	STATICLIB_RANGES_REFWRAP_HPP

#include <iterator>
#include <functional>
#include <utility>

namespace staticlib {
namespace ranges {

namespace detail_refwrap {

/**
 * Lazy `InputIterator` implementation for `std::ref`  operation.
 * Does not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Wraps elements from source iterator into std::reference_wrapper, 
 * and moves wrappers them out from `operator*` method.
 */
template<typename Iter, typename Elem>
class refwrapped_iter {
    Iter source_iter;

public:
    using value_type = std::reference_wrapper<Elem>;
    // does not support input_iterator, but valid tag is required
    // for std::iterator_traits with libc++ on mac
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::nullptr_t;
    using pointer = std::nullptr_t;
    using reference = std::nullptr_t;

    /**
     * Constructor
     * 
     * @param source source iterator
     */
    refwrapped_iter(Iter source_iter) :
    source_iter(std::move(source_iter)) { }
    
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    refwrapped_iter(const refwrapped_iter& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    refwrapped_iter& operator=(const refwrapped_iter& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    refwrapped_iter(refwrapped_iter&& other) :
    refwrapped_iter(std::move(other.source_iter)) { }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    refwrapped_iter& operator=(refwrapped_iter&& other) {
        this->source_iter = std::move(other.iter);
        return *this;
    }

    /**
     * Delegated prefix operator implementation
     * 
     * @return reference to iter instance
     */
    refwrapped_iter& operator++() {
        ++source_iter;
        return *this;
    }

    /**
     * Delegated postfix operator implementation
     * 
     * @return reference to iter instance
     */
    refwrapped_iter& operator++(int) {
        source_iter++;
        return *this;
    }

    /**
     * Clones element from source iterator and returns it.
     * 
     * @return transformed element
     */
    std::reference_wrapper<Elem> operator*() {
        auto& el = *source_iter;
        return std::ref(el);
    }

    /**
     * Delegated operator implementation, does NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end "past the end" iterator
     * @return whether not both this and specified iterators are "past the end"
     */
    bool operator!=(const refwrapped_iter& end) const {
        return this->source_iter != end.source_iter;
    }
};


/**
 * Lazy `InputIterator` implementation for `std::cref`  operation.
 * Do not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Wraps elements from source iterator into std::reference_wrapper, 
 * and moves wrappers them out from `operator*` method.
 */
template<typename Iter, typename Elem>
class refwrapped_const_iter  {
    Iter source_iter;

public:
    using value_type = std::reference_wrapper<const Elem>;
    // do not support input_iterator
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::nullptr_t;
    using pointer = std::nullptr_t;
    using reference = std::nullptr_t;

    /**
     * Constructor
     * 
     * @param source source iterator
     */
    refwrapped_const_iter(Iter source_iter) :
    source_iter(std::move(source_iter)) { }
    
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    refwrapped_const_iter(const refwrapped_const_iter & other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    refwrapped_const_iter& operator=(const refwrapped_const_iter & other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    refwrapped_const_iter(refwrapped_const_iter && other) :
    source_iter(std::move(other.source_iter)) { }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    refwrapped_const_iter& operator=(refwrapped_const_iter && other) {
        this->source_iter = std::move(other.source_iter);
        return *this;
    }

    /**
     * Delegated prefix operator implementation
     * 
     * @return reference to iter instance
     */
    refwrapped_const_iter& operator++() {
        ++source_iter;
        return *this;
    }

    /**
     * Delegated postfix operator implementation
     * 
     * @return reference to iter instance
     */
    refwrapped_const_iter& operator++(int) {
        source_iter++;
        return *this;
    }

    /**
     * Clones element from source iterator and returns it.
     * 
     * @return transformed element
     */
    std::reference_wrapper<const Elem> operator*() {
        const auto& el = *source_iter;
        return std::cref(el);
    }

    /**
     * Delegated operator implementation, do NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end "past the end" iterator
     * @return whether not both this and specified iterators are "past the end"
     */
    bool operator!=(const refwrapped_const_iter & end) const {
        return this->source_iter != end.source_iter;
    }
};

} // namespace

/**
 * Lazy implementation of `SinglePassRange` for `std::ref`  operation
 */
template <typename Range>
class refwrapped_range {
    Range& source_range;

public:
    /**
     * Type of iterator of this range
     */
    using iterator = decltype(std::declval<decltype(source_range)>().begin());

    /**
     * Result unwrapped value type of of iterators returned from this range
     */
    using value_type_unwrapped = typename std::iterator_traits<iterator>::value_type;

    /**
     * Result value type of iterators returned from this range
     */
    using value_type = typename std::reference_wrapper<value_type_unwrapped>;

    /**
     * Constructor,
     * created range wrapper will NOT own specified range
     * 
     * @param range reference to source range
     */
    refwrapped_range(Range& source_range) :
    source_range(source_range) { }

    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    refwrapped_range(const refwrapped_range& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    refwrapped_range& operator=(const refwrapped_range& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    refwrapped_range(refwrapped_range&& other) :
    source_range(other.source_range) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    refwrapped_range& operator=(refwrapped_range&& other) = delete;

    /**
     * Returns `begin` transformed iterator
     * 
     * @return `begin` iterator
     */
    detail_refwrap::refwrapped_iter<iterator, value_type_unwrapped> begin() {
        return detail_refwrap::refwrapped_iter<iterator, value_type_unwrapped>{std::move(source_range.begin())};
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    detail_refwrap::refwrapped_iter<iterator, value_type_unwrapped> end() {
        return detail_refwrap::refwrapped_iter<iterator, value_type_unwrapped>{std::move(source_range.end())};
    }
};


/**
 * Lazy implementation of `SinglePassRange` for `std::cref`  operation
 */
template <typename Range>
class refwrapped_const_range {
    const Range& source_range;

public:
    /**
     * Type of iterator of this range
     */
    using iterator = decltype(std::declval<decltype(source_range)>().begin());

    /**
     * Result unwrapped value type of of iterators returned from this range
     */
    using value_type_unwrapped = typename std::iterator_traits<iterator>::value_type;

    /**
     * Result value type of iterators returned from this range
     */
    using value_type = typename std::reference_wrapper<value_type_unwrapped>;

    /**
     * Constructor,
     * created range wrapper will NOT own specified range
     * 
     * @param range reference to source range
     */
    refwrapped_const_range(const Range& source_range) :
    source_range(source_range) { }

    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    refwrapped_const_range(const refwrapped_const_range& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    refwrapped_const_range& operator=(const refwrapped_const_range& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    refwrapped_const_range(refwrapped_const_range&& other) :
    source_range(other.source_range) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    refwrapped_const_range& operator=(refwrapped_const_range&& other) = delete;

    /**
     * Returns `begin` transformed iterator
     * 
     * @return `begin` iterator
     */
    detail_refwrap::refwrapped_const_iter<iterator, value_type_unwrapped> begin() {
        return detail_refwrap::refwrapped_const_iter<iterator, value_type_unwrapped>{std::move(source_range.begin())};
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    detail_refwrap::refwrapped_const_iter<iterator, value_type_unwrapped> end() {
        return detail_refwrap::refwrapped_const_iter<iterator, value_type_unwrapped>{std::move(source_range.end())};
    }
};



/**
 * Lazily copies input range into output range using `std::ref` function on
 * each element.
 * Created range wrapper will NOT own specified range.
 * 
 * @param range source range
 * @return cloned range
 */
template <typename Range>
refwrapped_range<Range> refwrap(Range& range) {
    return refwrapped_range<Range>(range);
}

/**
 * Lazily copies input range into output range using `std::cref` function on
 * each element.
 * Created range wrapper will NOT own specified range.
 * 
 * @param range source range
 * @return cloned range
 */
template <typename Range>
refwrapped_const_range<Range> refwrap(const Range& range) {
    return refwrapped_const_range<Range>(range);
}

} // namespace
}

#endif	/* STATICLIB_RANGES_REFWRAP_HPP */

