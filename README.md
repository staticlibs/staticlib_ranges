Staticlibs ranges library
=========================

This project is a part of [Staticlibs](http://staticlibs.net/).

This project contains implementation for the lazy transformation wrappers for arbitrary ranges.
It is designed to work primarily with move-only non-copyable objects. 

This library is similar in nature with [cppitertools](https://github.com/ryanhaining/cppitertools) library
but much less powerful and much less complex. It was implemented before cppitertools got support for
move-only objects.

This library is header-only.

Link to [API documentation](http://staticlibs.github.io/staticlib_ranges/docs/html/namespacestaticlib_1_1ranges.html).

Lazy transformation ranges wrappers
-----------------------------------

This library works on arbitrary C++ ranges - the objects that can return `begin` and
`past_the_end` input iterators using `begin()` and `end()`methods.

Wrappers are destructive to source ranges - they use `std::move` to get the elements from the input iterator
and returns elements to callers from its own iterators also using `std::move`. Because of this
wrapper iterators do not conform with `InputIterator` concepts. They still can be used with C++11 `foreach` syntax though.

To prevent the possibly-destructive `std::move` on source ranges `refwrap` wrapper can be used before
transformation wrappers: it will lazily wrap all the source elements using `std::ref` 
(`std::cref` for const source ranges) so following `std::move` will effectively operate on
`std::reference_wrapper` objects over source range elements.

This library implements the following operations:

 - `transform`: lazily transforms input range into output range lazily applying specified function to each element of source range
 - `filter`: lazily filters all the elements of source range using specified predicate function. 
Takes additional parameter - the function that will be applied to element that won't match the predicate
(to not lose this elements accidentally because of `std::move` application). If the "offcast" 
elements can be thrown away (for example, if `refwrap` wrapper is used before the `filter`) then
`ignore_offcast<T>` can be used as a last argument.
 - `concat`: lazily concatenates two ranges into single range. Wrapped range will iterate over first range
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
    std::vector<std::reference_wrapper<MyMovable>> res = sit::emplace_to_vector(concatted);

License information
-------------------

This project is released under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0)

Changelog
---------

**2015-05-19**

 * 1.0.0, initial public version
