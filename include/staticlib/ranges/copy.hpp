/* 
 * File:   copy.hpp
 * Author: alex
 *
 * Created on January 30, 2015, 11:09 AM
 */

#ifndef STATICLIB_COPY_HPP
#define	STATICLIB_COPY_HPP

#include <utility>
#include <iterator>

namespace staticlib {
namespace ranges {

/**
 * Lazy `InputIterator` implementation for `copy`  operation.
 * Do not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Copies element from source iterator, and moves them out from `operator*` method.
 */
template<typename I, typename E>
class copied_iter : public std::iterator<std::input_iterator_tag, E> {
    I source_iter;

public:
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    copied_iter(const copied_iter& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    copied_iter& operator=(const copied_iter& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    copied_iter(copied_iter&& other) :
    copied_iter(std::move(other.source_iter)) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    copied_iter& operator=(copied_iter&& other) = delete;

    /**
     * Constructor
     * 
     * @param source source iterator
     */
    copied_iter(I source_iter) :
    source_iter(std::move(source_iter)) { }

    /**
     * Delegated prefix operator implementation
     * 
     * @return reference to iter instance
     */
    copied_iter& operator++() {
        ++source_iter;
        return *this;
    }

    /**
     * Delegated postfix operator implementation
     * 
     * @return reference to iter instance
     */
    copied_iter& operator++(int) {
        source_iter++;
        return *this;
    }

    /**
     * Copies element from source iterator and returns it.
     * 
     * @return transformed element
     */
    E operator*() {
        E el{*source_iter};
        return *source_iter;
    }

    /**
     * Delegated operator implementation, do NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end
     * @return false if this iterator is exhausted
     */
    bool operator!=(const copied_iter& end) const {
        return this->source_iter != end.source_iter;
    }
};

/**
 * Lazy implementation of `SinglePassRange` for `copy`  operation
 */
template <typename R>
class copied_range {
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
    copied_range(const copied_range& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    copied_range& operator=(const copied_range& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    copied_range(copied_range&& other) :
    source_range(other.source_range) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    copied_range& operator=(copied_range&& other) = delete;

    /**
     * Constructor
     * 
     * @param range reference to source range
     */
    copied_range(const R& source_range) :
    source_range(source_range) { }

    /**
     * Returns `begin` transformed iterator
     * 
     * @return `begin` iterator
     */
    copied_iter<iterator, value_type> begin() {
        return copied_iter<iterator, value_type>{std::move(source_range.begin())};
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    copied_iter<iterator, value_type> end() {
        return copied_iter<iterator, value_type>{std::move(source_range.end())};
    }
};

/**
 * Lazily copies input range into output range using copy constructor on
 * each element.
 * 
 * @param range source range
 * @return copied range
 */
template <typename R>
copied_range<R> copy(const R& range) {
    return copied_range<R>(range);
}

} // namespace
}

#endif	/* STATICLIB_COPY_HPP */

