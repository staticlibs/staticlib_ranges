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
 * File:   transform.hpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:33 PM
 */

#ifndef STATICLIB_RANGES_TRANSFORM_HPP
#define STATICLIB_RANGES_TRANSFORM_HPP

#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "staticlib/ranges/refwrap.hpp"

namespace staticlib {
namespace ranges {

namespace detail_transform {

/**
 * Lazy `InputIterator` implementation for `transform`  operation.
 * Does not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Moves element from source iterator, applies `FunctionObject` (usually lambda) 
 * to it and moves it out from `operator*` method.
 */
template<typename Iter, typename Elem, typename Func>
class transformed_iter {
    Iter source_iter;
    Func* functor;

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
     * @param source source iterator
     * @param functor `FunctionObject` to apply to returned values
     */
    transformed_iter(Iter source_iter, Func& functor) :
    source_iter(std::move(source_iter)),
    functor(std::addressof(functor)) { }
    
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    transformed_iter(const transformed_iter& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    transformed_iter& operator=(const transformed_iter& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    transformed_iter(transformed_iter&& other) : 
    source_iter(std::move(other.source_iter)), 
    functor(std::move(other.functor)) { }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    transformed_iter& operator=(transformed_iter&& other) {
        this->source_iter = std::move(other.source_iter);
        this->functor = std::move(other.functor);
        return *this;
    }

    /**
     * Delegated prefix operator implementation
     * 
     * @return reference to iter instance
     */
    transformed_iter& operator++() {
        ++source_iter;
        return *this;
    }

    /**
     * Delegated postfix operator implementation
     * 
     * @return reference to iter instance
     */
    transformed_iter& operator++(int) {
        source_iter++;
        return *this;
    }

    /**
     * Moves element from source iterator, applies functor (usually lambda) 
     * to it and returns it.
     * 
     * @return transformed element
     */
    Elem operator*() {
        return (*functor)(std::move(*source_iter));
    }

    /**
     * Delegated operator implementation, does NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end "past the end" iterator
     * @return whether not both this and specified iterators are "past the end"
     */
    bool operator!=(const transformed_iter& end) const {
        return this->source_iter != end.source_iter;
    }
};

} // namespace


/**
 * Lazy implementation of `SinglePassRange` for `transform`  operation, 
 * after the pass all accessed elements of source range will be moved from
 * (will retain in "valid but unspecified" state).
 */
template <typename Range, typename Func>
class transformed_range {
    Range source_range;
    Func functor;

public:
    /**
     * Type of iterator of source range
     */
    using source_iterator = decltype(std::declval<decltype(source_range)>().begin());
           
    /**
     * Result value type of iterators returned from this range
     */
    // https://connect.microsoft.com/VisualStudio/feedback/details/797682/c-decltype-of-class-member-access-incompletely-implemented
    using value_type = decltype(std::declval<decltype(functor)>()(std::move(*std::declval<decltype(source_range)>().begin())));
    
    /**
     * Result iterator type
     */
    using iterator = detail_transform::transformed_iter<source_iterator, value_type, Func>;

    /**
     * Constructor, 
     * created range wrapper will own specified ranges
     * 
     * @param range reference to source range
     * @param functor transformation `FunctionObject`, can be move-only
     */
    transformed_range(Range&& source_range, Func functor) :
    source_range(std::move(source_range)),
    functor(std::move(functor)) { }
    
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    transformed_range(const transformed_range& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    transformed_range& operator=(const transformed_range& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    transformed_range(transformed_range&& other) :
    source_range(std::move(other.source_range)), 
    functor(std::move(other.functor)) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    transformed_range& operator=(transformed_range&& other) = delete;

    /**
     * Returns `begin` transformed iterator
     * 
     * @return `begin` iterator
     */
    detail_transform::transformed_iter<source_iterator, value_type, Func> begin() {
        return detail_transform::transformed_iter<source_iterator, value_type, Func>{std::move(source_range.begin()), functor};
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    detail_transform::transformed_iter<source_iterator, value_type, Func> end() {
        return detail_transform::transformed_iter<source_iterator, value_type, Func>{std::move(source_range.end()), functor};
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
 * Lazily transforms input range into output range applying functor to
 * each element. Elements are moved from source range one by one,
 * All accessed elements of source range will be left in "valid but unspecified state".
 * 
 * @param range source range
 * @param functor transformation `FunctionObject`, can be move-only
 * @return transformed range
 */
template <typename Range, typename Func,
        class = typename std::enable_if<!std::is_lvalue_reference<Range>::value>::type>
transformed_range<Range, Func> transform(Range&& range, Func functor) {
    return transformed_range<Range, Func>(std::move(range), std::move(functor));
}

/**
 * Lazily transforms input range into output range applying functor to
 * each element taking it by reference.
 * 
 * @param range source range
 * @param functor transformation `FunctionObject`, can be move-only
 * @return transformed range
 */
template <typename Range, typename Func>
transformed_range<staticlib::ranges::refwrapped_range<Range>, Func> transform(Range& range, Func functor) {
    return transform(staticlib::ranges::refwrap(range), std::move(functor));
}

/**
 * Lazily transforms input range into output range applying functor to
 * each element taking it by reference.
 * 
 * @param range source range
 * @param functor transformation `FunctionObject`, can be move-only
 * @return transformed range
 */
template <typename Range, typename Func>
transformed_range<staticlib::ranges::refwrapped_const_range<Range>, Func> transform(const Range& range, Func functor) {
    return transform(staticlib::ranges::refwrap(range), std::move(functor));
}

} // namespace
}

#endif    /* STATICLIB_RANGES_TRANSFORM_HPP */

