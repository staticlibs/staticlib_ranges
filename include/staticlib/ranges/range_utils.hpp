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
 * File:   range_utils.hpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:34 PM
 */

#ifndef STATICLIB_RANGES_RANGES_UTILS_HPP
#define	STATICLIB_RANGES_RANGES_UTILS_HPP

#include <iterator>
#include <functional>
#include <vector>

namespace staticlib {
namespace ranges {

/**
 * Moves all the elements from the specified range into vector using `emplace_back`
 * 
 * @param range range with `MoveConstructible` elements
 * @return vector containing all element from specified range
 */
template <typename R>
auto emplace_to_vector(R&& range) -> std::vector<typename std::iterator_traits<decltype(range.begin())>::value_type> {
    auto vec = std::vector<typename std::iterator_traits<decltype(range.begin())>::value_type>{};
    // resize is not used here, as neither 'transformed' nor 'filtered' 
    // range will have O(1) size available
    for (auto&& el : range) {
        vec.emplace_back(std::move(el));
    }
    return vec;
}

/**
 * Moves all the elements from the specified range into specified destination
 * using `emplace_back`.
 * 
 * @param dest destination container
 * @param range source range
 * @return destination container
 */
template <typename D, typename R>
D& emplace_to(D& dest, R&& range) {
    for (auto&& el : range) {
        dest.emplace_back(std::move(el));
    }
    return dest;
}

/**
 * Utility function to use as an offcast `FunctionObject` argument for `filter` function.
 * Discards all offcast elements.
 * 
 * @param t offcast object
 */
template <typename T>
void ignore_offcast(T t) {
    (void) t; // ignored
}

/**
 * Utility function to use as an offcast `FunctionObject` argument for `filter` function.
 * Emplaces all offcast elements into specified container
 * 
 * @param dest container to emplace offcast elements into
 * @return `FunctionObject` argument for `filter` function
 */
template <typename T, typename E = typename T::value_type>
std::function<void(E)> offcast_into(T& dest) {
    return [&dest](E el) {
        dest.emplace_back(std::move(el));
    };
}

/**
 * `any` algorithm implementation for the arbitrary ranges
 * 
 * @param range input range
 * @param predicate function to check range elements with
 * @return true if function returned true on some range element,
 *         false otherwise
 */
template <typename R, typename P>
bool any(R& range, P predicate) {
    for (auto&& el : range) {
        auto& ref = el;
        if (predicate(ref)) {
            return true;
        }
    }
    return false;
}

/**
 * `find` algorithm implementation for the arbitrary ranges,
 *  found element will be `move-returned` to the caller
 * 
 * @param range input range
 * @param predicate function to check range elements with
 * @param not_found_el this element will be returned if 
 *        predicate won't match any element
 * @return range element that will match the predicate, 
 *         `not_found_el` argument if no elements will match
 */
template <typename R, typename P, typename E>
E find(R& range, P predicate, E not_found_el) {
    for (auto&& el : range) {
        auto& ref = el;
        if (predicate(ref)) {
            return std::move(el);
        }
    }
    return std::move(not_found_el);
}


} // namespace
}

#endif	/* STATICLIB_RANGES_RANGES_UTILS_HPP */
