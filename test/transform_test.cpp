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
#include <map>
#include <memory>
#include <sstream>
#include <vector>

#include "staticlib/config/assert.hpp"
#include "staticlib/support.hpp"

#include "staticlib/ranges/concat.hpp"
#include "staticlib/ranges/filter.hpp"
#include "staticlib/ranges/range_utils.hpp"
#include "staticlib/ranges/refwrap.hpp"

#include "domain_classes.hpp"


void test_vector() {
    auto vec = std::vector<std::unique_ptr<my_int>>{};
    vec.emplace_back(new my_int(40));
    vec.emplace_back(new my_int(41));
    
    auto range = sl::ranges::transform(std::move(vec), [](std::unique_ptr<my_int> el) {
        return std::unique_ptr<my_str>(new my_str(sl::support::to_string(el->get_int() - 10)));
    });

    auto res = range.to_vector();

    slassert(2 == res.size());
    slassert("30" == res[0]->get_str());
    slassert("31" == res[1]->get_str());
}

void test_range() {
    auto vec1 = std::vector<std::unique_ptr<my_int>>{};
    vec1.emplace_back(new my_int(40));
    vec1.emplace_back(new my_int(41));
    
    auto vec2 = std::vector<std::unique_ptr<my_str>>{};
    vec2.emplace_back(new my_str("52"));
    vec2.emplace_back(new my_str("53"));
    vec2.emplace_back(new my_str("54"));
    
    auto range1 = sl::ranges::transform(std::move(vec1), [](std::unique_ptr<my_int> el) {
        return std::unique_ptr<my_str>(new my_str(sl::support::to_string(el->get_int() + 10)));
    });
    auto range2 = sl::ranges::concat(std::move(range1), std::move(vec2));
    auto range3 = sl::ranges::filter(std::move(range2), [](std::unique_ptr<my_str>& el) {
        return "52" != el->get_str();
    }, sl::ranges::ignore_offcast<std::unique_ptr<my_str>>);
    auto range4 = sl::ranges::transform(std::move(range3), [](std::unique_ptr<my_str> el) {
        return std::unique_ptr<my_str>(new my_str(el->get_str() + "_42"));
    });
    auto res = range4.to_vector();

    slassert(4 == res.size());
    slassert("50_42" == res[0]->get_str());
    slassert("51_42" == res[1]->get_str());
    slassert("53_42" == res[2]->get_str());
    slassert("54_42" == res[3]->get_str());
}

void test_map() {
    std::map<const std::string, std::unique_ptr<my_int>> map{};
    map.insert(std::pair<const std::string, std::unique_ptr<my_int>>("foo", std::unique_ptr<my_int>(new my_int(41))));
    map.insert(std::pair<const std::string, std::unique_ptr<my_int>>("bar", std::unique_ptr<my_int>(new my_int(42))));
    map.insert(std::pair<const std::string, std::unique_ptr<my_int>>("baz", std::unique_ptr<my_int>(new my_int(43))));
    auto wrapped = sl::ranges::refwrap(map);
    auto ra = sl::ranges::transform(std::move(wrapped), [](std::pair<const std::string, std::unique_ptr<my_int>>& el) {
        return el.second->get_int();
    });
    auto res = ra.to_vector();
    slassert(3 == res.size());
}

void test_lvalue() {
    auto vec = std::vector<std::unique_ptr<my_int>>();
    vec.emplace_back(new my_int(40));
    vec.emplace_back(new my_int(41));
    const auto& vecref = vec;
    auto ra = sl::ranges::transform(vecref, [](const std::unique_ptr<my_int>& el) {
        return el->get_int();
    });
    auto res = ra.to_vector();
    slassert(2 == res.size());
    slassert(40 == res[0]);
    slassert(41 == res[1]);
}

void test_readme() {
    using namespace std;
    using namespace staticlib::ranges;
    
    // prepare two ranges (containers) with move-only objects
    vector<my_movable> vec{};
    vec.emplace_back(my_movable(41));
    vec.emplace_back(my_movable(42));
    vec.emplace_back(my_movable(43));

    list<my_movable> li{};
    li.emplace_back(91);
    li.emplace_back(92);

    // take vector by reference and transform each element and return it through `reference_wrapper`
    // transformation also can return new object (of different type) if required
    auto transformed = transform(vec, [](my_movable& el) {
        el.set_val(el.get_val() + 10);
        return std::ref(el);
    });

    // filter the elements
    auto filtered = filter(transformed, [](my_movable& el) {
        return 52 != el.get_val();
    });

    // do transformation over filtered range
    auto transformed2 = transform(filtered, [](my_movable& el) {
        el.set_val(el.get_val() - 20);
        return std::ref(el);
    });

    // use "refwrap" to guard second source container against `std::move`
    auto refwrapped = refwrap(li);
    // concatenate two ranges
    auto concatted = concat(transformed2, refwrapped);

    // evaluate all operations and store results in vector ("auto res" will work here too)
    vector<reference_wrapper<my_movable>> res = concatted.to_vector();
    
    slassert(4 == res.size());
    slassert(31 == res[0].get().get_val());
    slassert(33 == res[1].get().get_val());
    slassert(91 == res[2].get().get_val());
    slassert(92 == res[3].get().get_val());
}

int main() {
    try {
        test_vector();
        test_range();
        test_map();
        test_lvalue();
        test_readme();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
