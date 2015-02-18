/* 
 * File:   range_utils.hpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:34 PM
 */

#ifndef STATICLIB_MOVE_UTILS_HPP
#define	STATICLIB_MOVE_UTILS_HPP

#include <vector>
#include <iterator>

namespace staticlib {
namespace ranges {

/**
 * Moves all the elements from specified range into vector using `emplace_back`
 * 
 * @param range range with `MoveConstructible` elements
 * @return vector containing all element from specified range
 */
template <typename R>
auto emplace_to_vector(R& range) -> std::vector<typename std::iterator_traits<decltype(range.begin())>::value_type> {
    auto vec = std::vector<typename std::iterator_traits<decltype(range.begin())>::value_type>{};
    // resize is not used here, as neither 'transformed' nor 'filtered' 
    // range will have O(1) size available
    for (auto&& el : range) {
        vec.emplace_back(std::move(el));
    }
    return vec;
}

/**
 * Moves all the elements from specified range into specified destination
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
std::function<void(E) > offcast_into(T& dest) {
    return [&dest](E el) {
        dest.emplace_back(std::move(el));
    };
}

} // namespace
}

#endif	/* STATICLIB_MOVE_UTILS_HPP */

