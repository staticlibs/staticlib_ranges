/* 
 * File:   clone.hpp
 * Author: alex
 *
 * Created on January 30, 2015, 11:09 AM
 */

#ifndef STATICLIB_CLONE_HPP
#define	STATICLIB_CLONE_HPP

#include <utility>
#include <iterator>

namespace staticlib {
namespace ranges {

/**
 * Lazy `InputIterator` implementation for `clone`  operation.
 * Do not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Clones elements from source iterator, and moves them out from `operator*` method.
 */
template<typename I, typename E>
class cloned_iter : public std::iterator<std::input_iterator_tag, E> {
    I source_iter;

public:
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    cloned_iter(const cloned_iter& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    cloned_iter& operator=(const cloned_iter& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    cloned_iter(cloned_iter&& other) :
    cloned_iter(std::move(other.source_iter)) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    cloned_iter& operator=(cloned_iter&& other) = delete;

    /**
     * Constructor
     * 
     * @param source source iterator
     */
    cloned_iter(I source_iter) :
    source_iter(std::move(source_iter)) { }

    /**
     * Delegated prefix operator implementation
     * 
     * @return reference to iter instance
     */
    cloned_iter& operator++() {
        ++source_iter;
        return *this;
    }

    /**
     * Delegated postfix operator implementation
     * 
     * @return reference to iter instance
     */
    cloned_iter& operator++(int) {
        source_iter++;
        return *this;
    }

    /**
     * Clones element from source iterator and returns it.
     * 
     * @return transformed element
     */
    E operator*() {
        return source_iter->clone();
    }

    /**
     * Delegated operator implementation, do NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end
     * @return false if this iterator is exhausted
     */
    bool operator!=(const cloned_iter& end) {
        return this->source_iter != end.source_iter;
    }
};

/**
 * Lazy implementation of `SinglePassRange` for `clone`  operation
 */
template <typename R>
class cloned_range {
    const R& source_range;

public:
    /**
     * Type of iterator of this range
     */
    typedef decltype(std::declval<decltype(source_range)>().begin()) iterator;

    /**
     * Result value type of iterators returned from this range
     */
    typedef typename std::iterator_traits<iterator>::value_type value_type;

    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    cloned_range(const cloned_range& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    cloned_range& operator=(const cloned_range& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    cloned_range(cloned_range&& other) :
    source_range(other.source_range) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    cloned_range& operator=(cloned_range&& other) = delete;

    /**
     * Constructor
     * 
     * @param range reference to source range
     */
    cloned_range(const R& source_range) :
    source_range(source_range) { }

    /**
     * Returns `begin` transformed iterator
     * 
     * @return `begin` iterator
     */
    cloned_iter<iterator, value_type> begin() {
        return cloned_iter<iterator, value_type>{std::move(source_range.begin())};
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    cloned_iter<iterator, value_type> end() {
        return cloned_iter<iterator, value_type>{std::move(source_range.end())};
    }
};

/**
 * Lazily copies input range into output range using `clone` method on
 * each element.
 * 
 * @param range source range
 * @return cloned range
 */
template <typename R>
cloned_range<R> clone(const R& range) {
    return cloned_range<R>(range);
}

} // namespace
}

#endif	/* STATICLIB_CLONE_HPP */

