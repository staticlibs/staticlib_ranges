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

#include <iostream>
#include <cassert>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <array>

#include "domain_classes.hpp"
#include "staticlib/ranges.hpp"

namespace { //anonymous

namespace sit = staticlib::ranges;

void test_state_after_move() {
    auto a = MyMovable(42);
    auto b = std::move(a);
    assert(42 == b.get_val());
    assert(-1 == a.get_val());
}

void test_transform_refwrapped() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(MyMovable(41));
    vec.emplace_back(MyMovable(42));
    vec.emplace_back(MyMovable(43));
    
    std::list<MyMovable> li{};
    li.emplace_back(91);
    li.emplace_back(92);

    auto refwrapped = sit::refwrap(vec);
    auto transformed = sit::transform(refwrapped, [](MyMovable& el) {
        el.set_val(el.get_val() + 10);
        return std::ref(el);
    });
    auto filtered = sit::filter(transformed, [](MyMovable& el) {
        return 52 != el.get_val();
    }, sit::ignore_offcast<MyMovable&>);
    auto transformed2 = sit::transform(filtered, [](MyMovable& el) {
        el.set_val(el.get_val() - 10);
        return std::ref(el);
    });
    auto refwrapped2 = sit::refwrap(li);
    auto concatted = sit::concat(transformed2, refwrapped2);

    //auto res will work too
    std::vector<std::reference_wrapper<MyMovable>> res = sit::emplace_to_vector(concatted);
    assert(4 == res.size());
    assert(41 == res[0].get().get_val());
    assert(43 == res[1].get().get_val());
    assert(91 == res[2].get().get_val());
    assert(92 == res[3].get().get_val());

    assert(3 == vec.size());
    assert(41 == vec[0].get_val());
    assert(52 == vec[1].get_val());
    assert(43 == vec[2].get_val());

    assert(2 == li.size());
    assert(91 == li.begin()->get_val());
}

void test_refwrap_to_value() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);
    
    auto refwrapped = sit::refwrap(vec);
    auto transformed = sit::transform(refwrapped, [](MyMovable& el) {
        return std::move(el);
    });
    auto filtered = sit::filter(transformed, [](MyMovable& el) {
        return 42 != el.get_val();
    }, sit::ignore_offcast<MyMovable>);
    
    auto res = sit::emplace_to_vector(filtered);
    assert(2 == res.size());
    assert(41 == res[0].get_val());
    assert(43 == res[1].get_val());

    assert(3 == vec.size());
    assert(-1 == vec[0].get_val());
    assert(-1 == vec[1].get_val());
    assert(-1 == vec[2].get_val());
}

void test_const_ref() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(MyMovable(41));
    vec.emplace_back(MyMovable(42));
    vec.emplace_back(MyMovable(43));
    
    const std::vector<MyMovable> vec2 = std::move(vec);
    const std::vector<MyMovable>& vec2ref = vec2;
    
    auto refwrapped = sit::refwrap(vec2ref);
    int sum = 0;
    auto transformed1 = sit::transform(refwrapped, [&sum](const MyMovable& el) {
        sum += el.get_val();
        return std::ref(el);
    });
    auto filtered = sit::filter(transformed1, [](const MyMovable& el) {
        (void) el;
        return false;
    }, sit::ignore_offcast<const MyMovable&>);
    auto res = sit::emplace_to_vector(filtered);
    (void) res;
    
    std::cout << sum << std::endl;
}
    

} // namespace

int main() {
    test_state_after_move();
    test_transform_refwrapped();
    test_refwrap_to_value();
    test_const_ref();

    return 0;
}

