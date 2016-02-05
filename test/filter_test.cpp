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

namespace ra = staticlib::ranges;

void test_vector() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    vec.emplace_back(new MyInt(42));
    vec.emplace_back(new MyInt(43));
    
    auto offcasted = std::vector<std::unique_ptr<MyInt>>{};
    auto range = ra::filter(std::move(vec), [](std::unique_ptr<MyInt>& el) {
        return 42 == el->get_int();
    }, ra::offcast_into(offcasted));

    auto res = ra::emplace_to_vector(std::move(range));

    slassert(1 == res.size());
    slassert(42 == res[0]->get_int());

    slassert(3 == offcasted.size());
    slassert(40 == offcasted[0]->get_int());
    slassert(41 == offcasted[1]->get_int());
    slassert(43 == offcasted[2]->get_int());
}

void test_range() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    auto list = std::list<std::unique_ptr<MyInt>>{};
    list.emplace_back(new MyInt(42));
    list.emplace_back(new MyInt(43));
    auto range = ra::concat(std::move(vec), std::move(list));
    
    auto filtered = ra::filter(std::move(range), [](std::unique_ptr<MyInt>& el) {
        return el->get_int() <= 40;
    }, ra::ignore_offcast<std::unique_ptr<MyInt>>);
    auto res = ra::emplace_to_vector(std::move(filtered));

    slassert(1 == res.size());
    slassert(40 == res[0]->get_int());
}

void test_non_default_constructible() {
    auto vec = std::vector<MyMovable>{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);
    
    auto filtered = ra::filter(std::move(vec), [](MyMovable& el) {
        return 42 != el.get_val();
    }, ra::ignore_offcast<MyMovable>);

    auto res = ra::emplace_to_vector(std::move(filtered));

    slassert(2 == res.size());
    slassert(41 == res[0].get_val());
    slassert(43 == res[1].get_val());
}

void test_moved() {
    auto vec = std::vector<MyMovable>{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);

    auto filtered = ra::filter(std::move(vec), [](MyMovable & el) {
        return 42 != el.get_val();
    }, ra::ignore_offcast<MyMovable>);
    
    auto res = ra::emplace_to_vector(std::move(filtered));
    
    slassert(vec.empty());
    slassert(2 == res.size());
    slassert(41 == res[0].get_val());
    slassert(43 == res[1].get_val());
}

int main() {
    try {
        test_vector();
        test_range();
        test_non_default_constructible();
        test_moved();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

