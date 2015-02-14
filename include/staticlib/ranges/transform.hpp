/* 
 * File:   transform.hpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:33 PM
 */

#ifndef STATICLIB_TRANSFORM_HPP
#define STATICLIB_TRANSFORM_HPP

#include <utility>
#include <iterator>

namespace staticlib {
namespace ranges {

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
    source_iter(std::move(other.source_iter)), functor(other.functor) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    transformed_iter& operator=(transformed_iter&& other) = delete;

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
    typedef decltype(std::declval<decltype(source_range)>().begin()) source_iterator;

    /**
     * Result value type of iterators returned from this range
     */
    // https://connect.microsoft.com/VisualStudio/feedback/details/797682/c-decltype-of-class-member-access-incompletely-implemented
    typedef decltype(std::declval<decltype(functor)>()(std::move(*std::declval<decltype(source_range)>().begin()))) value_type;

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
    source_range(other.source_range), functor(std::move(other.functor)) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    transformed_range& operator=(transformed_range&& other) = delete;

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
        return transformed_iter<source_iterator, value_type, F>{std::move(source_range.begin()), functor};
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    transformed_iter<source_iterator, value_type, F> end() {
        return transformed_iter<source_iterator, value_type, F>{std::move(source_range.end()), functor};
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

#endif    /* STATICLIB_TRANSFORM_HPP */

