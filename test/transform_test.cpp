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
 * File:   transform_test.cpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:35 PM
 */

#include "staticlib/ranges/transform.hpp"

#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <vector>

#include "staticlib/config/assert.hpp"
#include "staticlib/config/to_string.hpp"

#include "staticlib/ranges/concat.hpp"
#include "staticlib/ranges/filter.hpp"
#include "staticlib/ranges/range_utils.hpp"

#include "domain_classes.hpp"


namespace sc = staticlib::config;
namespace sit = staticlib::ranges;

void test_vector() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    
    auto range = sit::transform(vec, [](std::unique_ptr<MyInt> el) {
        return std::unique_ptr<MyStr>(new MyStr(sc::to_string(el->get_int() - 10)));
    });

    auto res = sit::emplace_to_vector(range);

    slassert(2 == res.size());
    slassert("30" == res[0]->get_str());
    slassert("31" == res[1]->get_str());
}

void test_range() {
    auto vec1 = std::vector<std::unique_ptr<MyInt>>{};
    vec1.emplace_back(new MyInt(40));
    vec1.emplace_back(new MyInt(41));
    
    auto vec2 = std::vector<std::unique_ptr<MyStr>>{};
    vec2.emplace_back(new MyStr("52"));
    vec2.emplace_back(new MyStr("53"));
    vec2.emplace_back(new MyStr("54"));
    
    auto range1 = sit::transform(vec1, [](std::unique_ptr<MyInt> el) {
        return std::unique_ptr<MyStr>(new MyStr(sc::to_string(el->get_int() + 10)));
    });
    auto range2 = sit::concat(range1, vec2);
    auto range3 = sit::filter(range2, [](std::unique_ptr<MyStr>& el) {
        return "52" != el->get_str();
    }, sit::ignore_offcast<std::unique_ptr<MyStr>>);
    auto range4 = sit::transform(range3, [](std::unique_ptr<MyStr> el) {
        return std::unique_ptr<MyStr>(new MyStr(el->get_str() + "_42"));
    });
    auto res = sit::emplace_to_vector(range4);

    slassert(4 == res.size());
    slassert("50_42" == res[0]->get_str());
    slassert("51_42" == res[1]->get_str());
    slassert("53_42" == res[2]->get_str());
    slassert("54_42" == res[3]->get_str());
}

int main() {
    try {
        test_vector();
        test_range();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
