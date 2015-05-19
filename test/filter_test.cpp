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

#include <cassert>
#include <vector>
#include <list>
#include <memory>

#include "domain_classes.hpp"
#include "staticlib/ranges.hpp"

namespace { //anonymous

namespace mv = staticlib::ranges;

void test_vector() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    vec.emplace_back(new MyInt(42));
    vec.emplace_back(new MyInt(43));
    
    auto offcasted = std::vector<std::unique_ptr<MyInt>>{};
    auto range = mv::filter(vec, [](std::unique_ptr<MyInt>& el) {
        return 42 == el->get_int();
    }, mv::offcast_into(offcasted));

    auto res = mv::emplace_to_vector(range);

    assert(1 == res.size());
    assert(42 == res[0]->get_int());

    assert(3 == offcasted.size());
    assert(40 == offcasted[0]->get_int());
    assert(41 == offcasted[1]->get_int());
    assert(43 == offcasted[2]->get_int());
}

void test_range() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    auto list = std::list<std::unique_ptr<MyInt>>{};
    list.emplace_back(new MyInt(42));
    list.emplace_back(new MyInt(43));
    auto range = mv::concat(vec, list);
    
    auto filtered = mv::filter(range, [](std::unique_ptr<MyInt>& el) {
        return el->get_int() <= 40;
    }, mv::ignore_offcast<std::unique_ptr<MyInt>>);
    auto res = mv::emplace_to_vector(filtered);

    assert(1 == res.size());
    assert(40 == res[0]->get_int());
}

void test_non_default_constructible() {
    auto vec = std::vector<MyMovable>{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);
    
    auto filtered = mv::filter(vec, [](MyMovable& el) {
        return 42 != el.get_val();
    }, mv::ignore_offcast<MyMovable>);

    auto res = mv::emplace_to_vector(filtered);

    assert(2 == res.size());
    assert(41 == res[0].get_val());
    assert(43 == res[1].get_val());
}

} // namespace
 
int main() {
    test_vector();
    test_range();
    test_non_default_constructible();

    return 0;
}

