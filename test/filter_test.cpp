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
 * File:   filter_test.cpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:35 PM
 */

#include "staticlib/ranges/filter.hpp"

#include <iostream>
#include <list>
#include <memory>
#include <vector>

#include "staticlib/config/assert.hpp"

#include "staticlib/ranges/concat.hpp"
#include "staticlib/ranges/range_utils.hpp"

#include "domain_classes.hpp"

void test_vector() {
    auto vec = std::vector<std::unique_ptr<my_int>>{};
    vec.emplace_back(new my_int(40));
    vec.emplace_back(new my_int(41));
    vec.emplace_back(new my_int(42));
    vec.emplace_back(new my_int(43));
    
    auto offcasted = std::vector<std::unique_ptr<my_int>>{};
    auto range = sl::ranges::filter(std::move(vec), [](std::unique_ptr<my_int>& el) {
        return 42 == el->get_int();
    }, sl::ranges::offcast_into(offcasted));

    auto res = range.to_vector();

    slassert(1 == res.size());
    slassert(42 == res[0]->get_int());

    slassert(3 == offcasted.size());
    slassert(40 == offcasted[0]->get_int());
    slassert(41 == offcasted[1]->get_int());
    slassert(43 == offcasted[2]->get_int());
}

void test_range() {
    auto vec = std::vector<std::unique_ptr<my_int>>{};
    vec.emplace_back(new my_int(40));
    vec.emplace_back(new my_int(41));
    auto list = std::list<std::unique_ptr<my_int>>{};
    list.emplace_back(new my_int(42));
    list.emplace_back(new my_int(43));
    auto range = sl::ranges::concat(std::move(vec), std::move(list));
    
    auto filtered = sl::ranges::filter(std::move(range), [](std::unique_ptr<my_int>& el) {
        return el->get_int() <= 40;
    }, sl::ranges::ignore_offcast<std::unique_ptr<my_int>>);
    auto res = filtered.to_vector();

    slassert(1 == res.size());
    slassert(40 == res[0]->get_int());
}

void test_non_default_constructible() {
    auto vec = std::vector<my_movable>{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);
    
    auto filtered = sl::ranges::filter(std::move(vec), [](my_movable& el) {
        return 42 != el.get_val();
    }, sl::ranges::ignore_offcast<my_movable>);

    auto res = filtered.to_vector();

    slassert(2 == res.size());
    slassert(41 == res[0].get_val());
    slassert(43 == res[1].get_val());
}

void test_moved() {
    auto vec = std::vector<my_movable>{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);

    auto filtered = sl::ranges::filter(std::move(vec), [](my_movable & el) {
        return 42 != el.get_val();
    }, sl::ranges::ignore_offcast<my_movable>);
    
    auto res = filtered.to_vector();
    
    slassert(vec.empty());
    slassert(2 == res.size());
    slassert(41 == res[0].get_val());
    slassert(43 == res[1].get_val());
}

void test_lvalue() {
    auto vec = std::vector<my_movable>{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);
    const auto& vecref = vec;

    auto filtered = sl::ranges::filter(vecref, [](const my_movable & el) {
        return 42 != el.get_val();
    });
    auto res = filtered.to_vector();

    slassert(3 == vec.size());
    slassert(41 == vec[0].get_val());
    slassert(42 == vec[1].get_val());
    slassert(43 == vec[2].get_val());
    slassert(2 == res.size());
    slassert(41 == res[0].get().get_val());
    slassert(43 == res[1].get().get_val());
}

int main() {
    try {
        test_vector();
        test_range();
        test_non_default_constructible();
        test_moved();
        test_lvalue();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

