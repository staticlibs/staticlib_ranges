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


void test_state_after_move() {
    auto a = my_movable(42);
    auto b = std::move(a);
    slassert(42 == b.get_val());
    slassert(-1 == a.get_val());
}

void test_transform_refwrapped() {
    std::vector<my_movable> vec{};
    vec.emplace_back(my_movable(41));
    vec.emplace_back(my_movable(42));
    vec.emplace_back(my_movable(43));
    
    std::list<my_movable> li{};
    li.emplace_back(91);
    li.emplace_back(92);

    auto transformed = sl::ranges::transform(sl::ranges::refwrap(vec), [](my_movable& el) {
        el.set_val(el.get_val() + 10);
        return std::ref(el);
    });
    auto filtered = sl::ranges::filter(std::move(transformed), [](my_movable& el) {
        return 52 != el.get_val();
    }, sl::ranges::ignore_offcast<my_movable&>);
    auto transformed2 = sl::ranges::transform(std::move(filtered), [](my_movable& el) {
        el.set_val(el.get_val() - 10);
        return std::ref(el);
    });
    auto refwrapped2 = sl::ranges::refwrap(li);
    auto concatted = sl::ranges::concat(std::move(transformed2), std::move(refwrapped2));

    //auto res will work too
    std::vector<std::reference_wrapper<my_movable>> res = concatted.to_vector();
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
    std::vector<my_movable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);
    
    auto transformed = sl::ranges::transform(sl::ranges::refwrap(vec), [](my_movable& el) {
        return std::move(el);
    });
    auto filtered = sl::ranges::filter(std::move(transformed), [](my_movable& el) {
        return 42 != el.get_val();
    }, sl::ranges::ignore_offcast<my_movable>);
    
    auto res = filtered.to_vector();
    slassert(2 == res.size());
    slassert(41 == res[0].get_val());
    slassert(43 == res[1].get_val());

    slassert(3 == vec.size());
    slassert(-1 == vec[0].get_val());
    slassert(-1 == vec[1].get_val());
    slassert(-1 == vec[2].get_val());
}

void test_const_ref() {
    std::vector<my_movable> vec{};
    vec.emplace_back(my_movable(41));
    vec.emplace_back(my_movable(42));
    vec.emplace_back(my_movable(43));
    
    const std::vector<my_movable> vec2 = std::move(vec);
    const std::vector<my_movable>& vec2ref = vec2;
    
    int sum = 0;
    auto transformed1 = sl::ranges::transform(sl::ranges::refwrap(vec2ref), [&sum](const my_movable& el) {
        sum += el.get_val();
        return std::ref(el);
    });
    auto filtered = sl::ranges::filter(std::move(transformed1), [](const my_movable&) {
        return false;
    }, sl::ranges::ignore_offcast<const my_movable&>);
    auto res = filtered.to_vector();
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
