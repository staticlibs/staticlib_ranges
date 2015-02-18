/* 
 * File:   refwrap.hpp
 * Author: alex
 *
 * Created on February 16, 2015, 10:58 AM
 */

#ifndef STATICLIB_REFWRAP_HPP
#define	STATICLIB_REFWRAP_HPP

#include <utility>
#include <iterator>
#include <functional>

namespace staticlib {
namespace ranges {

namespace detail {

/**
 * Lazy `InputIterator` implementation for `std::ref`  operation.
 * Do not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Wraps elements from source iterator into std::reference_wrapper, 
 * and moves wrappers them out from `operator*` method.
 */
template<typename I, typename E>
class refwrapped_iter {
    I source_iter;

public:
    typedef std::reference_wrapper<E> value_type;
    // do not support input_iterator
    typedef std::nullptr_t iterator_category;
    typedef std::nullptr_t difference_type;
    typedef std::nullptr_t pointer;
    typedef std::nullptr_t reference;
    
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
     * Constructor
     * 
     * @param source source iterator
     */
    refwrapped_iter(I source_iter) :
    source_iter(std::move(source_iter)) { }

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
    std::reference_wrapper<E> operator*() {
        E& el = *source_iter;
        return std::ref(el);
    }

    /**
     * Delegated operator implementation, do NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end
     * @return false if this iterator is exhausted
     */
    bool operator!=(const refwrapped_iter& end) const {
        return this->source_iter != end.source_iter;
    }
};

/**
 * Lazy implementation of `SinglePassRange` for `std::ref`  operation
 */
template <typename R>
class refwrapped_range {
    R& source_range;

public:
    /**
     * Type of iterator of this range
     */
    typedef decltype(std::declval<decltype(source_range)>().begin()) iterator;

    /**
     * Result unwrapped value type of of iterators returned from this range
     */
    typedef typename std::iterator_traits<iterator>::value_type value_type_unwrapped;

    /**
     * Result value type of iterators returned from this range
     */
    typedef typename std::reference_wrapper<value_type_unwrapped> value_type;

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
     * Constructor
     * 
     * @param range reference to source range
     */
    refwrapped_range(R& source_range) :
    source_range(source_range) { }

    /**
     * Returns `begin` transformed iterator
     * 
     * @return `begin` iterator
     */
    refwrapped_iter<iterator, value_type_unwrapped> begin() {
        return refwrapped_iter<iterator, value_type_unwrapped>{std::move(source_range.begin())};
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    refwrapped_iter<iterator, value_type_unwrapped> end() {
        return refwrapped_iter<iterator, value_type_unwrapped>{std::move(source_range.end())};
    }
};


/**
 * Lazy `InputIterator` implementation for `std::ref`  operation.
 * Do not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Wraps elements from source iterator into std::reference_wrapper, 
 * and moves wrappers them out from `operator*` method.
 */
template<typename I, typename E>
class refwrapped_const_iter  {
    I source_iter;

public:
    typedef std::reference_wrapper<const E> value_type;
    // do not support input_iterator
    typedef std::nullptr_t iterator_category;
    typedef std::nullptr_t difference_type;
    typedef std::nullptr_t pointer;
    typedef std::nullptr_t reference;
        
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
     * Constructor
     * 
     * @param source source iterator
     */
    refwrapped_const_iter(I source_iter) :
    source_iter(std::move(source_iter)) { }

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
    std::reference_wrapper<const E> operator*() {
        const E& el = *source_iter;
        return std::cref(el);
    }

    /**
     * Delegated operator implementation, do NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end
     * @return false if this iterator is exhausted
     */
    bool operator!=(const refwrapped_const_iter & end) const {
        return this->source_iter != end.source_iter;
    }
};

/**
 * Lazy implementation of `SinglePassRange` for `std::ref`  operation
 */
template <typename R>
class refwrapped_const_range {
    const R& source_range;

public:
    /**
     * Type of iterator of this range
     */
    typedef decltype(std::declval<decltype(source_range)>().begin()) iterator;

    /**
     * Result unwrapped value type of of iterators returned from this range
     */
    typedef typename std::iterator_traits<iterator>::value_type value_type_unwrapped;

    /**
     * Result value type of iterators returned from this range
     */
    typedef typename std::reference_wrapper<value_type_unwrapped> value_type;

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
     * Constructor
     * 
     * @param range reference to source range
     */
    refwrapped_const_range(const R& source_range) :
    source_range(source_range) { }

    /**
     * Returns `begin` transformed iterator
     * 
     * @return `begin` iterator
     */
    refwrapped_const_iter<iterator, value_type_unwrapped> begin() {
        return refwrapped_const_iter<iterator, value_type_unwrapped>{std::move(source_range.begin())};
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    refwrapped_const_iter<iterator, value_type_unwrapped> end() {
        return refwrapped_const_iter<iterator, value_type_unwrapped>{std::move(source_range.end())};
    }
};

} // namespace

/**
 * Lazily copies input range into output range using `std::ref` function on
 * each element.
 * 
 * @param range source range
 * @return cloned range
 */
template <typename R>
detail::refwrapped_range<R> refwrap(R& range) {
    return detail::refwrapped_range<R>(range);
}

template <typename R>
detail::refwrapped_const_range<R> refwrap(const R& range) {
    return detail::refwrapped_const_range<R>(range);
}

} // namespace
}

#endif	/* STATICLIB_REFWRAP_HPP */

