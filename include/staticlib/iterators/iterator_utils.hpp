/* 
 * File:   iterator_utils.hpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:34 PM
 */

#ifndef ITERATOR_UTILS_HPP
#define	ITERATOR_UTILS_HPP

#include <vector>
#include <iterator>

namespace staticlib {
namespace iterators {

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

} // namespace
}

#endif	/* ITERATOR_UTILS_HPP */

