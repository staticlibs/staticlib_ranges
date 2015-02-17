/* 
 * File:   filter.hpp
 * Author: alex
 *
 * Created on February 17, 2015, 9:13 AM
 */

#ifndef STATICLIB_REF_FILTER_HPP
#define	STATICLIB_REF_FILTER_HPP

#include <utility>
#include <iterator>
#include <functional>
#include <memory>

namespace staticlib {
namespace ranges {
namespace ref {

namespace detail {

/**
 * Lazy `InputIterator` implementation for `filter`  operation.
 * Accesses element by reference from source iterator, checks it against specified `Predicate`
 * and on success returns it out from `operator*` method.
 */
template <typename I, typename E, typename P>
class filtered_iter : public std::iterator<std::input_iterator_tag, E> {
    I source_iter;
    I source_iter_end;
    P* predicate;
    E* current_ptr;

public:
    /**
     * Copy constructor
     *
     * @param other other instance
     */
    filtered_iter(const filtered_iter& other) : 
    source_iter(other.source_iter),
    source_iter_end(other.source_iter_end),
    predicate(other.predicate),
    current_ptr(other.current_ptr) { }

    /**
     * Copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    filtered_iter& operator=(const filtered_iter& other) {
        this->source_iter = other.source_iter;
        this->source_iter_end = other.source_iter_end;
        this->predicate = other.predicate;
        this->current_ptr = other.current_ptr;
        return *this;
    }

    /**
     * Move constructor
     *
     * @param other other instance
     */
    filtered_iter(filtered_iter&& other) :
    source_iter(std::move(other.source_iter)),
    source_iter_end(std::move(other.source_iter_end)),
    predicate(other.predicate),
    current_ptr(std::move(other.current_ptr)) { }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    filtered_iter& operator=(filtered_iter&& other) {
        this->source_iter = std::move(other.source_iter);
        this->source_iter_end = std::move(other.source_iter_end);
        this->predicate = std::move(other.predicate);
        this->current_ptr = std::move(other.current_ptr);
        return *this;
    }

    /**
     * Constructor
     * 
     * @param source_iter source `begin` iterator
     * @param source_iter_end source `past_the_end` iterator
     * @param predicate filtering `Predicate`
     */
    filtered_iter(I source_iter, I source_iter_end, P& predicate) :
    source_iter(std::move(source_iter)),
    source_iter_end(std::move(source_iter_end)),
    predicate(&predicate),
    current_ptr(nullptr) {
        if (this->source_iter != this->source_iter_end) {
            E& el = *this->source_iter;
            this->current_ptr = &el;
            if (!this->predicate(*current_ptr)) {
                next();
            }
        }
    }

    /**
     * Will iterate over source elements until successful `Predicate`
     * application or source exhaustion.
     * Offcast elements will be applied to `FunctionObject`
     * 
     * @return reference to this iterator
     */
    filtered_iter& operator++() {
        next();
        return *this;
    }

    /**
     * Will iterate over source elements until successful `Predicate`
     * application or source exhaustion.
     * Offcast elements will be applied to `FunctionObject
     * 
     * @return reference to this iterator
     */
    filtered_iter& operator++(int) {
        next();
        return *this;
    }

    /**
     * Will return reference to current element that matched predicate
     * 
     * @return current element
     */
    E& operator*() const {
        E& el = *current_ptr;
        return el;
    }

    /**
     * Delegated operator implementation, do NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end
     * @return false if this iterator is exhausted
     */
    bool operator!=(const filtered_iter& end) const {
        return this->source_iter != end.source_iter;
    }

private:

    void next() {
        for (++source_iter; source_iter != source_iter_end; ++source_iter) {
            E& el = *source_iter;
            current_ptr = &el;
            if (predicate(*current_ptr)) break;
        }
    }

};

/**
 * Lazy implementation of `SinglePassRange` for `filter`  operation.
 * 
 */
template <typename R, typename P>
class filtered_range {
    R& source_range;
    P predicate;

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
    filtered_range(const filtered_range& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    filtered_range& operator=(const filtered_range& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    filtered_range(filtered_range&& other) :
    source_range(other.source_range), 
    predicate(std::move(other.predicate)) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    filtered_range& operator=(filtered_range&& other) = delete;

    /**
     * Constructor
     * 
     * @param source_range reference to source range
     * @param predicate `Predicate` to check source element againt it
     */
    filtered_range(R& source_range, P predicate) :
    source_range(source_range), 
    predicate(std::move(predicate)) { }

    /**
     * Returns `begin` filtered iterator
     * 
     * @return `begin` iterator
     */
    filtered_iter<iterator, value_type, P> begin() {
        return filtered_iter<iterator, value_type, P>{
            std::move(source_range.begin()), std::move(source_range.end()), predicate
        };
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    filtered_iter<iterator, value_type, P> end() {
        return filtered_iter<iterator, value_type, P>{
            std::move(source_range.end()), std::move(source_range.end()), predicate
        };
    }
};

} // namespace

/**
 * Lazily filters input range into output range checking each element using
 * specified `Predicate`.
 * 
 * @param source_range reference to source range
 * @param predicate `Predicate` to check source element againt it
 * @return filtered range
 */
template <typename T, typename P>
detail::filtered_range<T, P> filter(T& source_range, P predicate) {
    return detail::filtered_range<T, P>(source_range, std::move(predicate));
}


} // namespace
}
}

#endif	/* STATICLIB_REF_FILTER_HPP */

