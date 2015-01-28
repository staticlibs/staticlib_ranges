/* 
 * File:   transform.hpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:33 PM
 */

#ifndef TRANSFORM_HPP
#define	TRANSFORM_HPP

#include <utility>
#include <iterator>

namespace staticlib {
namespace iterators {

/**
 * Lazy `InputIterator` implementation for `transform`  operation.
 * Do not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Moves element from source iterator, applies `FunctionObject` (usually lambda) 
 * to it and moves it out from `operator*` method.
 */
template<typename I, typename E, typename F>
class transformed_iter : public std::iterator<std::input_iterator_tag, E> {
    I source_iter;
    F& functor;

public:
    /**
     * Constructor
     * 
     * @param source source iterator
     * @param functor `FunctionObject` to apply to returned values
     */
    transformed_iter(I source_iter, F& functor) : 
    source_iter(std::move(source_iter)), functor(functor) { }

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
    E operator*() {
        return functor(std::move(*source_iter));
    }

    /**
     * Delegated operator implementation, do NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end
     * @return false if this iterator is exhausted
     */
    bool operator!=(const transformed_iter& end) {
        return this->source_iter != end.source_iter;
    }
};


/**
 * Lazy implementation of `SinglePassRange` for `transform`  operation, 
 * after the pass all accessed elements of source range will be moved from
 * (will retain in "valid but unspecified" state).
 */
template <typename R, typename F>
class transformed_range {
    R& source_range;
    F functor;

public:
    /**
     * Type of iterator of source range
     */
    typedef decltype(source_range.begin()) source_iterator;
    
    /**
     * Result value type of iterators returned from this range
     */
    typedef decltype(functor(std::move(*source_range.begin()))) value_type;

    /**
     * Constructor
     * 
     * @param range reference to source range
     * @param functor transformation `FunctionObject`, can be move-only
     */
    transformed_range(R& source_range, F functor) : 
    source_range(source_range), functor(std::move(functor)) { }

    /**
     * Returns `begin` transformed iterator
     * 
     * @return `begin` iterator
     */
    transformed_iter<source_iterator, value_type, F> begin() {
        return transformed_iter<source_iterator, value_type, F>{source_range.begin(), functor};
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    transformed_iter<source_iterator, value_type, F> end() {
        return transformed_iter<source_iterator, value_type, F>{source_range.end(), functor};
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
template <typename R, typename F>
transformed_range<R, F> transform(R& range, F functor) {
    return transformed_range<R, F>(range, std::move(functor));
}

} // namespace
}

#endif	/* TRANSFORM_HPP */

