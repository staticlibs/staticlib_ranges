/* 
 * File:   concat.hpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:33 PM
 */

#ifndef STATICLIB_CONCAT_HPP
#define STATICLIB_CONCAT_HPP

#include <utility>
#include <iterator>

namespace staticlib {
namespace ranges {

/**
 * Lazy `InputIterator` implementation for `concat` (or `chain`) operation.
 * Do not support `CopyConstructible`, `CopyAssignable` and `Swappable`.
 * Moves element from source iterators one by one and moves it out from `operator*` method.
 */
template<typename I1, typename I2, typename E>
class concatted_iter : public std::iterator<std::input_iterator_tag, E> {
    I1 source_iter1;
    I1 source_iter1_end;
    I2 source_iter2;

public:
    /**
    * Deleted copy constructor
    *
    * @param other other instance
    */
    concatted_iter(const concatted_iter& other) = delete;

    /**
    * Deleted copy assignment operator
    *
    * @param other other instance
    * @return reference to this instance
    */
    concatted_iter& operator=(const concatted_iter& other) = delete;

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
    * Deleted move assignment operator
    *
    * @param other other instance
    * @return reference to this instance
    */
    concatted_iter& operator=(concatted_iter&& other) = delete;

    /**
     * Constructor
     * 
     * @param source_iter1 `begin` first source iterator
     * @param source_iter1_end `past_the_end` first source iterator
     * @param source_iter2 `begin` second iterator
     */
    concatted_iter(I1 source_iter1, I1 source_iter1_end, I2 source_iter2) :
    source_iter1(std::move(source_iter1)), source_iter1_end(std::move(source_iter1_end)),
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
     * Will move out current element
     * 
     * @return current element
     */
    E operator*() {
        if (source_iter1 != source_iter1_end) {
            return std::move(*source_iter1);
        } else {
            return std::move(*source_iter2);
        }
    }

    /**
     * Delegated operator implementation, do NOT support arbitrary input instances,
     * should be used only to compare with `past_the_end` iterator.
     * 
     * @param end
     * @return false if this iterator is exhausted
     */    
    bool operator!=(const concatted_iter& end) {
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
 * Lazy implementation of `SinglePassRange` for `concat` (or `chain`)  operation, 
 * after the pass all accessed elements of source ranges will be moved from
 * (will retain in "valid but unspecified" state).
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

/**
 * Lazily concatenates two input ranges into single output range.
 * Elements are moved from source ranges one by one,
 * All accessed elements of source ranges will be left in "valid but unspecified state".
 * 
 * @param source_range1 first source range
 * @param source_range2 second source range
 * @return concatenated range
 */
template <typename R1, typename R2>
concatted_range<R1, R2> concat(R1& range1, R2& range2) {
    return concatted_range<R1, R2>(range1, range2);
}

} // namespace
}

#endif    /* STATICLIB_CONCAT_HPP */

