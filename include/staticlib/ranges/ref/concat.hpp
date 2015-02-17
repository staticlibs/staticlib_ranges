/* 
 * File:   concat.hpp
 * Author: alex
 *
 * Created on February 17, 2015, 9:13 AM
 */

#ifndef STATICLIB_REF_CONCAT_HPP
#define	STATICLIB_REF_CONCAT_HPP

#include <utility>
#include <iterator>

namespace staticlib {
namespace ranges {
namespace ref {

namespace detail {

/**
 * Lazy `InputIterator` implementation for `concat` (or `chain`) operation.
 * Takes elements from source iterators by reference one by one and return them
 * by reference out from `operator*` method.
 */
template<typename I1, typename I2, typename E>
class concatted_iter : public std::iterator<std::input_iterator_tag, E> {
    I1 source_iter1;
    I1 source_iter1_end;
    I2 source_iter2;

public:
    /**
     * Copy constructor
     *
     * @param other other instance
     */
    concatted_iter(const concatted_iter& other) : 
    source_iter1(other.source_iter1),
    source_iter1_end(other.source_iter1_end),
    source_iter2(other.source_iter2) { }

    /**
     * Copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    concatted_iter& operator=(const concatted_iter& other) {
        this->source_iter1 = other.source_iter1;
        this->source_iter1_end = other.source_iter1_end;
        this->source_iter2 = other.source_iter2;
    }

    /**
     * Move constructor
     *
     * @param other other instance
     */
    concatted_iter(concatted_iter&& other) :
    source_iter1(std::move(other.source_iter1)),
    source_iter1_end(std::move(other.source_iter1_end)),
    source_iter2(std::move(other.source_iter2)) { }

    /**
     * Move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    concatted_iter& operator=(concatted_iter&& other) {
        this->source_iter1 = std::move(other.source_iter1);
        this->source_iter1_end = std::move(other.source_iter1_end);
        this->source_iter2 = std::move(other.source_iter2);
        return *this;
    }

    /**
     * Constructor
     * 
     * @param source_iter1 `begin` first source iterator
     * @param source_iter1_end `past_the_end` first source iterator
     * @param source_iter2 `begin` second iterator
     */
    concatted_iter(I1 source_iter1, I1 source_iter1_end, I2 source_iter2) :
    source_iter1(std::move(source_iter1)), 
    source_iter1_end(std::move(source_iter1_end)),
    source_iter2(std::move(source_iter2)) { }

    /**
     * Increments first source iterator if it is not yet exhausted, 
     * otherwise - increments second source iterator.
     * 
     * @return reference to this iterator
     */
    concatted_iter& operator++() {
        next();
        return *this;
    }

    /**
     * Increments first source iterator if it is not yet exhausted, 
     * otherwise - increments second source iterator.
     * 
     * @return reference to this iterator
     */
    concatted_iter& operator++(int) {
        next();
        return *this;
    }

    /**
     * Will return reference to current element
     * 
     * @return current element
     */
    E& operator*() const {
        if (source_iter1 != source_iter1_end) {
            E& el = *source_iter1;
            return el;
        } else {
            E& el = *source_iter2;
            return el;
        }
    }

    /**
     * Delegated operator implementation, do NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end
     * @return false if this iterator is exhausted
     */
    bool operator!=(const concatted_iter& end) const {
        return this->source_iter1 != end.source_iter1 ||
                this->source_iter2 != end.source_iter2;
    }

private:

    void next() {
        if (source_iter1 != source_iter1_end) {
            ++source_iter1;
        } else {
            ++source_iter2;
        }
    }
};

/**
 * Lazy implementation of `SinglePassRange` for `concat` (or `chain`)  operation.
 * 
 */
template <typename R1, typename R2>
class concatted_range {
    R1& source_range1;
    R2& source_range2;

public:
    /**
     * Type of iterator of first source range
     */
    typedef decltype(std::declval<decltype(source_range1)>().begin()) iterator1;
    /**
     * Type of iterator of second source range
     */
    typedef decltype(std::declval<decltype(source_range2)>().begin()) iterator2;
    /**
     * Result value type of iterators returned from this range
     */
    typedef typename std::iterator_traits<iterator1>::value_type value_type;

    /**
     * Deleted copy constructor
     *
     * @param other other instance
     */
    concatted_range(const concatted_range& other) = delete;

    /**
     * Deleted copy assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    concatted_range& operator=(const concatted_range& other) = delete;

    /**
     * Move constructor
     *
     * @param other other instance
     */
    concatted_range(concatted_range&& other) :
    source_range1(other.source_range1), source_range2(other.source_range2) { };

    /**
     * Deleted move assignment operator
     *
     * @param other other instance
     * @return reference to this instance
     */
    concatted_range& operator=(concatted_range&& other) = delete;

    /**
     * Constructor
     * 
     * @param source_range1 first source range
     * @param source_range2 second source range
     */
    concatted_range(R1& source_range1, R2& source_range2) :
    source_range1(source_range1), source_range2(source_range2) { }

    /**
     * Returns `begin` concatenated iterator
     * 
     * @return `begin` iterator
     */
    concatted_iter<iterator1, iterator2, value_type> begin() {
        // move here is required by msvs
        return concatted_iter<iterator1, iterator2, value_type>{
            std::move(source_range1.begin()), std::move(source_range1.end()), std::move(source_range2.begin())
        };
    }

    /**
     * Returns `past_the_end` iterator
     * 
     * @return `past_the_end` iterator
     */
    concatted_iter<iterator1, iterator2, value_type> end() {
        return concatted_iter<iterator1, iterator2, value_type>{
            std::move(source_range1.end()), std::move(source_range1.end()), std::move(source_range2.end())
        };
    }
};

} // namespace

/**
 * Lazily concatenates two input ranges into single output range.
 * 
 * @param source_range1 first source range
 * @param source_range2 second source range
 * @return concatenated range
 */
template <typename R1, typename R2>
detail::concatted_range<R1, R2> concat(R1& range1, R2& range2) {
    return detail::concatted_range<R1, R2>(range1, range2);
}

} // namespace
}
}

#endif	/* STATICLIB_REF_CONCAT_HPP */

