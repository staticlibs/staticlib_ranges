/* 
 * File:   filter.hpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:34 PM
 */

#ifndef STATICLIB_FILTER_HPP
#define STATICLIB_FILTER_HPP

#include <utility>
#include <iterator>
#include <functional>

namespace staticlib {
namespace ranges {

/**
 * Lazy `InputIterator` implementation for `filter`  operation.
 * Do not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Moves element from source iterator, checks it against specified `Predicate`
 * and on success moves it out from `operator*` method.
 * Elements, that do not match predicate will be applied to specified `FunctionObject`.
 */
template <typename I, typename E, typename P, typename D>
class filtered_iter : public std::iterator<std::input_iterator_tag, E> {
    I source_iter;
    I source_iter_end;
    P& predicate;
    D& offcast_dest;

    E current;
    
public:
    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    filtered_iter(const filtered_iter& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    filtered_iter& operator=(const filtered_iter& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    filtered_iter(filtered_iter&& other) :
        source_iter(std::move(other.source_iter)),
        source_iter_end(std::move(other.source_iter_end)),
        predicate(other.predicate),
        offcast_dest(other.offcast_dest),
        current(std::move(other.current)) { }

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    filtered_iter& operator=(filtered_iter&& other) = delete;

    /**
     * Constructor
     * 
     * @param source_iter source `begin` iterator
     * @param source_iter_end source `past_the_end` iterator
     * @param predicate filtering `Predicate`
     * @param offcast_dest `FunctionObject` for offcast elements
     */
    filtered_iter(I source_iter, I source_iter_end, P& predicate, D& offcast_dest) :
    source_iter(std::move(source_iter)),
    source_iter_end(std::move(source_iter_end)),
    predicate(predicate),
    offcast_dest(offcast_dest) {
        if (this->source_iter != this->source_iter_end) {
            current = std::move(*this->source_iter);
            if (!this->predicate(current)) {
                this->offcast_dest(std::move(current));
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
     * Will move out current element that matched predicate
     * 
     * @return current element
     */
    E operator*() {
        return std::move(current);
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
            current = std::move(*source_iter);
            if (predicate(current)) break;
            this->offcast_dest(std::move(current));
        }
    }
    
};


/**
 * Lazy implementation of `SinglePassRange` for `filter`  operation, 
 * after the pass all accessed elements of source range will be moved from
 * (will retain in "valid but unspecified" state). Elements that won't match the 
 * `Predicate` will be applied to specified `FunctionObject`
 */
template <typename R, typename P, typename D>
class filtered_range {
    R& source_range;
    P predicate;
    D offcast_dest;

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
    source_range(other.source_range), predicate(std::move(other.predicate)),
    offcast_dest(std::move(other.offcast_dest)) { }

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
     * @param offcast_dest `FunctionObject` to apply offcast elements to it
     */
    filtered_range(R& source_range, P predicate, D offcast_dest) : 
    source_range(source_range), predicate(std::move(predicate)), 
    offcast_dest(std::move(offcast_dest)) { }

    /**
     * Returns `begin` filtered iterator
     * 
     * @return `begin` iterator
     */
    filtered_iter<iterator, value_type, P, D> begin() {
        return filtered_iter<iterator, value_type, P, D>{
            std::move(source_range.begin()), std::move(source_range.end()), predicate, offcast_dest
        };
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    filtered_iter<iterator, value_type, P, D> end() {
        return filtered_iter<iterator, value_type, P, D>{
            std::move(source_range.end()), std::move(source_range.end()), predicate, offcast_dest
        };
    }
};


/**
 * Lazily filters input range into output range checking each element using
 * specified `Predicate`. Elements are moved from source range one by one,
 * All accessed elements of source range will be left in "valid but unspecified state".
 * Elements that won't match the `Predicate` will be applied to specified `FunctionObject`
 * 
 * @param source_range reference to source range
 * @param predicate `Predicate` to check source element againt it
 * @param offcast_dest `FunctionObject` to apply offcast elements to it
 * @return filtered range
 */
template <typename T, typename P, typename D>
filtered_range<T, P, D> filter(T& source_range, P predicate, D offcast_dest) {
    return filtered_range<T, P, D>(source_range, std::move(predicate), std::move(offcast_dest));
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


} // namespace
}

#endif    /* STATICLIB_FILTER_HPP */

