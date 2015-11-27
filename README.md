Staticlibs Ranges library
=========================

This project is a part of [Staticlibs](http://staticlibs.net/).

This project implements lazy transformation wrappers for arbitrary ranges, `range_adapter` for
"single-method" range implementations and some range-utility functions.

All range wrappers don't allocate dynamic memory on heap and were designed to be used primarily
with move-only non-copyable objects. 

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
(due to `Iterators` being "move-only" object) and generally can **NOT** be used with the standard STL algorithms.

All `Range`s/`Iterator`s in this library implement "destructive" value-semantics when on `Iterator`
dereference element is "move-returned" (using `std::move`) to the caller by value. This allows to 
use common lazy range operations like `transform` and `filter` on move-only non-copyable objects.

If required, reference ("non-destructive") logic can be achieved wrapping the `Range` with 
`std::reference_wrapper` lazy transformation using `refwrap` function.

Range adapter
-------------

`range_adapter` template can be inherited using [CRTP](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)
for the easy implementation of the `Range` with "move-return" semantics. Inheritors should implement 
a single method `compute_next` that should set next element as `current` using `set_current` and return `true`,
or return `false` if `Range` is exhausted:

    class MyRange : public range_adapter<MyRange, MyElem> {
    public:
        bool compute_next() {
            if (<has next>) {
                return this->set_current(<next element>);
            } else {
                return false;
            }
        }
    };

Lazy transformation range wrappers
----------------------------------

All the following range wrappers are implemented without heap memory allocation.

Wrappers are destructive to source ranges - they use `std::move` to get the elements from the input iterator
and returns elements to callers from its own iterators also using `std::move`.

To prevent the possibly-destructive `std::move` on source ranges `refwrap` wrapper can be used before
transformation wrappers: it will lazily wrap all the source elements using `std::ref` 
(`std::cref` for const source ranges) so following `std::move` will effectively operate on
`std::reference_wrapper` objects over unchanged source range elements.

####transform 

Lazily transforms input range into output range lazily applying specified function to each element of the source range.
 
####filter 

Lazily filters all the elements of source range using specified predicate function. 

Takes additional parameter - the function that will be applied to element that won't match the predicate
(to not lose this elements accidentally because of `std::move` application). If the "offcast" 
elements can be thrown away (for example, if `refwrap` wrapper is used before the `filter`) then
`ignore_offcast<T>` can be used as a last argument.
 
####concat 

Lazily concatenates two ranges into single range. Wrapped range will iterate over first range
until its exhaustion, then over the second range. Both input ranges must have elements of the same type.

Usage example
-------------

*Note: In this example namespaces are omitted for brevity*

    // prepare two ranges (containers) with move-only objects
    vector<MyMovable> vec{};
    vec.emplace_back(MyMovable(41));
    vec.emplace_back(MyMovable(42));
    vec.emplace_back(MyMovable(43));
    
    list<MyMovable> li{};
    li.emplace_back(91);
    li.emplace_back(92);

    // use "rewrap" to guard first source container against `std::move`
    auto refwrapped = refwrap(vec);

    // transform element and return it through `reference_wrapper`
    // transformation also can return new object (of different type) if required
    auto transformed = transform(refwrapped, [](MyMovable& el) {
        el.set_val(el.get_val() + 10);
        return std::ref(el);
    });

    // filter the elements
    auto filtered = filter(transformed, [](MyMovable& el) {
        return 52 != el.get_val();
    }, ignore_offcast<MyMovable&>);

    // do transformation over filtered range
    auto transformed2 = transform(filtered, [](MyMovable& el) {
        el.set_val(el.get_val() - 10);
        return std::ref(el);
    });

    // use "rewrap" to guard second source container against `std::move`
    auto refwrapped2 = refwrap(li);
    // concatenate two ranges
    auto concatted = concat(transformed2, refwrapped2);

    // evaluate all operations and store results in vector ("auto res" will work here too)
    vector<reference_wrapper<MyMovable>> res = emplace_to_vector(concatted);

License information
-------------------

This project is released under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).

Changelog
---------

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
