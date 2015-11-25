/*
 * Copyright 2015, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   refwrap_test.cpp
 * Author: alex
 *
 * Created on February 16, 2015, 11:10 AM
 */

#include "staticlib/ranges/refwrap.hpp"

#include <array>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "staticlib/config/assert.hpp"

#include "staticlib/ranges/concat.hpp"
#include "staticlib/ranges/filter.hpp"
#include "staticlib/ranges/range_utils.hpp"
#include "staticlib/ranges/transform.hpp"

#include "domain_classes.hpp"


namespace ra = staticlib::ranges;

void test_state_after_move() {
    auto a = MyMovable(42);
    auto b = std::move(a);
    slassert(42 == b.get_val());
    slassert(-1 == a.get_val());
}

void test_transform_refwrapped() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(MyMovable(41));
    vec.emplace_back(MyMovable(42));
    vec.emplace_back(MyMovable(43));
    
    std::list<MyMovable> li{};
    li.emplace_back(91);
    li.emplace_back(92);

    auto refwrapped = ra::refwrap(vec);
    auto transformed = ra::transform(refwrapped, [](MyMovable& el) {
        el.set_val(el.get_val() + 10);
        return std::ref(el);
    });
    auto filtered = ra::filter(transformed, [](MyMovable& el) {
        return 52 != el.get_val();
    }, ra::ignore_offcast<MyMovable&>);
    auto transformed2 = ra::transform(filtered, [](MyMovable& el) {
        el.set_val(el.get_val() - 10);
        return std::ref(el);
    });
    auto refwrapped2 = ra::refwrap(li);
    auto concatted = ra::concat(transformed2, refwrapped2);

    //auto res will work too
    std::vector<std::reference_wrapper<MyMovable>> res = ra::emplace_to_vector(concatted);
    slassert(4 == res.size());
    slassert(41 == res[0].get().get_val());
    slassert(43 == res[1].get().get_val());
    slassert(91 == res[2].get().get_val());
    slassert(92 == res[3].get().get_val());

    slassert(3 == vec.size());
    slassert(41 == vec[0].get_val());
    slassert(52 == vec[1].get_val());
    slassert(43 == vec[2].get_val());

    slassert(2 == li.size());
    slassert(91 == li.begin()->get_val());
}

void test_refwrap_to_value() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);
    
    auto refwrapped = ra::refwrap(vec);
    auto transformed = ra::transform(refwrapped, [](MyMovable& el) {
        return std::move(el);
    });
    auto filtered = ra::filter(transformed, [](MyMovable& el) {
        return 42 != el.get_val();
    }, ra::ignore_offcast<MyMovable>);
    
    auto res = ra::emplace_to_vector(filtered);
    slassert(2 == res.size());
    slassert(41 == res[0].get_val());
    slassert(43 == res[1].get_val());

    slassert(3 == vec.size());
    slassert(-1 == vec[0].get_val());
    slassert(-1 == vec[1].get_val());
    slassert(-1 == vec[2].get_val());
}

void test_const_ref() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(MyMovable(41));
    vec.emplace_back(MyMovable(42));
    vec.emplace_back(MyMovable(43));
    
    const std::vector<MyMovable> vec2 = std::move(vec);
    const std::vector<MyMovable>& vec2ref = vec2;
    
    auto refwrapped = ra::refwrap(vec2ref);
    int sum = 0;
    auto transformed1 = ra::transform(refwrapped, [&sum](const MyMovable& el) {
        sum += el.get_val();
        return std::ref(el);
    });
    auto filtered = ra::filter(transformed1, [](const MyMovable&) {
        return false;
    }, ra::ignore_offcast<const MyMovable&>);
    auto res = ra::emplace_to_vector(filtered);
    
//    std::cout << sum << std::endl;
}

int main() {
    try {
        test_state_after_move();
        test_transform_refwrapped();
        test_refwrap_to_value();
        test_const_ref();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
