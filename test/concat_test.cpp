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
 * File:   concat_test.cpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:34 PM
 */

#include "staticlib/ranges/concat.hpp"

#include <iostream>
#include <vector>
#include <list>
#include <memory>

#include "staticlib/config/assert.hpp"

#include "staticlib/ranges/filter.hpp"
#include "staticlib/ranges/range_utils.hpp"
#include "staticlib/ranges/transform.hpp"
#include "domain_classes.hpp"


namespace ra = staticlib::ranges;

void test_containers() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));

    auto list = std::list<std::unique_ptr<MyInt>>{};
    list.emplace_back(new MyInt(42));
    list.emplace_back(new MyInt(43));
    
    auto concatted = ra::concat(vec, list);
    auto res = ra::emplace_to_vector(concatted);
    
    slassert(4 == res.size());
    slassert(40 == res[0]->get_int());
    slassert(41 == res[1]->get_int());
    slassert(42 == res[2]->get_int());
    slassert(43 == res[3]->get_int());
}

void test_empty_first() {
    auto vec_empty = std::vector<std::unique_ptr<MyInt>>{};
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    
    auto range = ra::concat(vec_empty, vec);
    auto res = ra::emplace_to_vector(range);
    
    slassert(2 == res.size());
    slassert(40 == res[0]->get_int());
    slassert(41 == res[1]->get_int());
}

void test_empty_second() {
    auto vec_empty = std::vector<std::unique_ptr<MyInt>>{};
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));

    auto range = ra::concat(vec, vec_empty);
    auto res = ra::emplace_to_vector(range);
    
    slassert(2 == res.size());
    slassert(40 == res[0]->get_int());
    slassert(41 == res[1]->get_int());
}

void test_empty_both() {
    auto vec_empty1 = std::vector<std::unique_ptr<MyInt>>{};
    auto vec_empty2 = std::vector<std::unique_ptr<MyInt>>{};

    auto range = ra::concat(vec_empty1, vec_empty2);
    auto res = ra::emplace_to_vector(range);
    
    slassert(0 == res.size());
}

void test_ranges() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));

    auto list = std::list<std::unique_ptr < MyInt>>{};
    list.emplace_back(new MyInt(42));
    list.emplace_back(new MyInt(43));
    list.emplace_back(new MyInt(44));
    
    auto transformed = ra::transform(vec, [](std::unique_ptr<MyInt> el) {
        return std::unique_ptr<MyInt>(new MyInt(el->get_int() - 10));
    });
    auto filtered = ra::filter(list, [](std::unique_ptr<MyInt>& el) {
        return 42 != el->get_int();
    }, ra::ignore_offcast<std::unique_ptr<MyInt>>);
    auto concatted = ra::concat(transformed, filtered);
    auto res = ra::emplace_to_vector(concatted);

    slassert(4 == res.size());
    slassert(30 == res[0]->get_int());
    slassert(31 == res[1]->get_int());
    slassert(43 == res[2]->get_int());
    slassert(44 == res[3]->get_int());
}

int main() {
    try {
        test_containers();
        test_empty_first();
        test_empty_second();
        test_empty_both();
        test_ranges();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}