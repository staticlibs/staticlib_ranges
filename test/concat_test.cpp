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

#include <cassert>
#include <vector>
#include <list>
#include <memory>
#include <iostream>

#include "domain_classes.hpp"
#include "staticlib/ranges.hpp"

namespace { // anonymous

namespace mv = staticlib::ranges;


void test_containers() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));

    auto list = std::list<std::unique_ptr<MyInt>>{};
    list.emplace_back(new MyInt(42));
    list.emplace_back(new MyInt(43));
    
    auto concatted = mv::concat(vec, list);
    auto res = mv::emplace_to_vector(concatted);
    
    assert(4 == res.size());
    assert(40 == res[0]->get_int());
    assert(41 == res[1]->get_int());
    assert(42 == res[2]->get_int());
    assert(43 == res[3]->get_int());
}

void test_empty_first() {
    auto vec_empty = std::vector<std::unique_ptr<MyInt>>{};
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    
    auto range = mv::concat(vec_empty, vec);
    auto res = mv::emplace_to_vector(range);
    
    assert(2 == res.size());
    assert(40 == res[0]->get_int());
    assert(41 == res[1]->get_int());
}

void test_empty_second() {
    auto vec_empty = std::vector<std::unique_ptr<MyInt>>{};
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));

    auto range = mv::concat(vec, vec_empty);
    auto res = mv::emplace_to_vector(range);
    
    assert(2 == res.size());
    assert(40 == res[0]->get_int());
    assert(41 == res[1]->get_int());
}

void test_empty_both() {
    auto vec_empty1 = std::vector<std::unique_ptr<MyInt>>{};
    auto vec_empty2 = std::vector<std::unique_ptr<MyInt>>{};

    auto range = mv::concat(vec_empty1, vec_empty2);
    auto res = mv::emplace_to_vector(range);
    
    assert(0 == res.size());
}

void test_ranges() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));

    auto list = std::list<std::unique_ptr < MyInt>>{};
    list.emplace_back(new MyInt(42));
    list.emplace_back(new MyInt(43));
    list.emplace_back(new MyInt(44));
    
    auto transformed = mv::transform(vec, [](std::unique_ptr<MyInt> el) {
        return std::unique_ptr<MyInt>(new MyInt(el->get_int() - 10));
    });
    auto filtered = mv::filter(list, [](std::unique_ptr<MyInt>& el) {
        return 42 != el->get_int();
    }, mv::ignore_offcast<std::unique_ptr<MyInt>>);
    auto concatted = mv::concat(transformed, filtered);
    auto res = mv::emplace_to_vector(concatted);

    assert(4 == res.size());
    assert(30 == res[0]->get_int());
    assert(31 == res[1]->get_int());
    assert(43 == res[2]->get_int());
    assert(44 == res[3]->get_int());
}

} // namespace

int main() {
    test_containers();
    test_empty_first();
    test_empty_second();
    test_empty_both();
    test_ranges();
    
    return 0;
}

