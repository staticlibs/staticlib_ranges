Non-allocating lazy operations over ranges for C++11
====================================================

[![travis](https://travis-ci.org/staticlibs/staticlib_ranges.svg?branch=master)](https://travis-ci.org/staticlibs/staticlib_ranges)
[![appveyor](https://ci.appveyor.com/api/projects/status/github/staticlibs/staticlib_ranges?svg=true)](https://ci.appveyor.com/project/staticlibs/staticlib-ranges)

This project is a part of [Staticlibs](http://staticlibs.net/).

This project implements lazy transformation wrappers for arbitrary ranges, `range_adapter` for
"single-method" range implementations and some range-utility functions.

All range wrappers don't allocate dynamic memory on heap and can be used
with move-only non-copyable objects and dynamic (generator-like) ranges.

This library is similar in nature with [cppitertools](https://github.com/ryanhaining/cppitertools) library
but much less powerful and much less complex.

This library is header-only and has no dependencies.

Link to the [API documentation](http://staticlibs.github.io/staticlib_ranges/docs/html/namespacestaticlib_1_1ranges.html).

Ranges
------

In this library `Range` means an arbitrary C++ object that can be used with the C++11 `foreach` loop:

    for (auto&& el : range) {
       ...
    }

Term `Range` in C++ also can have more specific meanings, see [Boost.Range](http://www.boost.org/doc/libs/1_59_0/libs/range/doc/html/range/concepts/overview.html)
and [range-v3](https://github.com/ericniebler/range-v3) libraries.

In this library all `Iterator`s returned from the `Range`s do **NOT** satisfy standard C++ [Iterator concept](http://en.cppreference.com/w/cpp/concept/Iterator)
(due to `Iterator`s being "move-only") and generally can **NOT** be used with the standard STL algorithms.

`Range`s/`Iterator`s in this library support consuming input ranges in both ways - "by reference" or "by value".

With "reference" (`lvalue`, optionally `const`) inputs, wrapped ranges will operate over (constant) references 
to underlying elements. 

With "value (`rvalue`) inputs - "destructive" value-semantics will be used, when on `Iterator`
dereference element is "move-returned" (using `std::move`) to the caller by value. This allows to 
use common lazy range operations like `transform` and `filter` over move-only non-copyable objects
produced dynamically as a result of transformation.

Lazy transformation range wrappers
----------------------------------

All the following range wrappers are implemented without heap memory allocation (excluding the 
utility `to_vector` method).

#### transform ####

Lazily transforms input range into output range lazily applying specified function to each element of the source range.
 
#### filter ####

Lazily filters all the elements of source range using specified predicate function. 

With `rvalue` inputs it takes additional parameter - the function that will be applied to element that won't match the predicate
(to not lose this elements accidentally because of `std::move` application). If such "offcast" 
elements can be thrown away then helper function `ignore_offcast<T>` can be used as a last argument.
 
#### concat ####

Lazily concatenates two ranges into single range. Wrapped range will iterate over first range
until its exhaustion, then over the second range. Both input ranges must have elements of the same type.

#### refwrap ####

A helper range wrapper that wraps each input element into `std::reference_wrapper`. It is 
used automatically by other ranges for `lvalue` inputs.

Usage example
-------------

    // prepare two ranges (containers) with move-only objects
    std::vector<my_movable> vec{};
    vec.emplace_back(my_movable(41));
    vec.emplace_back(my_movable(42));
    vec.emplace_back(my_movable(43));

    std::list<my_movable> li{};
    li.emplace_back(91);
    li.emplace_back(92);

    // take vector by reference and transform each element and return it through `reference_wrapper`
    // transformation also can return new object (of different type) if required
    auto transformed = sl::ranges::transform(vec, [](my_movable& el) {
        el.set_val(el.get_val() + 10);
        return std::ref(el);
    });

    // filter the elements
    auto filtered = sl::ranges::filter(transformed, [](my_movable& el) {
        return 52 != el.get_val();
    });

    // do transformation over filtered range
    auto transformed2 = sl::ranges::transform(filtered, [](my_movable& el) {
        el.set_val(el.get_val() - 20);
        return std::ref(el);
    });

    // use "refwrap" to guard second source container against `std::move`
    auto refwrapped = sl::ranges::refwrap(li);
    // concatenate two ranges
    auto concatted = sl::ranges::concat(transformed2, refwrapped);

    // evaluate all operations and store results in vector ("auto res" will work here too)
    std::vector<reference_wrapper<my_movable>> res = concatted.to_vector();

Range adapter
-------------

`range_adapter` template can be inherited using [CRTP](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
for the easy implementation of the `Range` with "move-return" semantics. Inheritors should implement 
a single method `compute_next` that should set next element as `current` using `set_current` and return `true`,
or return `false` if `Range` is exhausted:

    class my_range : public range_adapter<my_range, my_elem> {
    public:
        bool compute_next() {
            if (<has next>) {
                return this->set_current(<next element>);
            } else {
                return false;
            }
        }
    };

License information
-------------------

This project is released under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).

Changelog
---------

**2017-12-22**
 * version 1.3.2
 * vs2017 support

**2017-04-08**
 * version 1.3.1
 * minor deps changes

**2017-02-03**

 * support for `lvalue` reference inputs for all ranges
 * version 1.3.0

**2016-07-10**

 * replace `typedef` with type aliases
 * version 1.2.3

**2016-01-22**

 * version 1.2.2
 * minor CMake changes

**2015-11-27**

 * version 1.2.1
 * use aligned stack storage for temporary elements
 * `range_adapter` iterator simplified

**2015-11-26**

 * version 1.2.0
 * range adapter added

**2015-11-25**

 * version 1.1.2
 * headers ordering
 * tests cleanup

**2015-11-06**

 * version 1.1.1
 * cmake gen improvements for windows

**2015-09-18**

 * version 1.1.0 
 * make filter to not allocate on heap
 * add pkg-config descriptor

**2015-07-08**

 * version 1.0.2 - toolchains update

**2015-06-30**

 * version 1.0.1 - toolchains update

**2015-05-19**

 * 1.0.0, initial public version
