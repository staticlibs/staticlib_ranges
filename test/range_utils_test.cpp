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
 * File:   range_utils_test.cpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:35 PM
 */

#include "staticlib/ranges/range_utils.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "staticlib/config/assert.hpp"
#include "staticlib/config/to_string.hpp"

#include "staticlib/ranges/filter.hpp"
#include "staticlib/ranges/refwrap.hpp"
#include "staticlib/ranges/transform.hpp"

#include "domain_classes.hpp"

namespace sc = staticlib::config;
namespace ra = staticlib::ranges;

void test_vector() {
    auto vec = std::vector<std::unique_ptr<MyStr>>{};
    vec.emplace_back(new MyStr("foo"));
    vec.emplace_back(new MyStr("bar"));
    
    auto res = ra::emplace_to_vector(std::move(vec));

    slassert(2 == res.size());
    slassert("foo" == res[0]->get_str());
    slassert("bar" == res[1]->get_str());
}

void test_range() {
    auto vec = std::vector<std::unique_ptr<MyStr>>{};
    vec.emplace_back(new MyStr("foo"));
    vec.emplace_back(new MyStr("bar"));    
    vec.emplace_back(new MyStr("baz"));
    auto range = ra::filter(std::move(vec), [](std::unique_ptr<MyStr>& el) {
        return "bar" != el->get_str();
    }, ra::ignore_offcast<std::unique_ptr<MyStr>>);
    
    auto res = ra::emplace_to_vector(std::move(range));
    
    slassert(2 == res.size());
    slassert("foo" == res[0]->get_str());
    slassert("baz" == res[1]->get_str());
}

void test_emplace_to() {
    auto vec = std::vector<std::unique_ptr<MyStr>>{};
    vec.emplace_back(new MyStr("foo"));
    vec.emplace_back(new MyStr("bar"));

    auto range = ra::transform(std::move(vec), [](std::unique_ptr<MyStr> el) {
        return std::unique_ptr<MyStr>(new MyStr(el->get_str() + "_42"));
    });

    auto res = std::vector<std::unique_ptr<MyStr>>{};
    res.reserve(vec.size());
    ra::emplace_to(res, std::move(range));
    
    slassert(2 == res.size());
    slassert("foo_42" == res[0]->get_str());
    slassert("bar_42" == res[1]->get_str());
}

void test_any() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    const auto vec2 = std::move(vec);
    
    auto rvec = ra::refwrap(vec2);
    auto filtered1 = ra::filter(std::move(rvec), [](const MyMovable& el) {
        return el.get_val() >= 42;
    }, ra::ignore_offcast<const MyMovable&>);
    bool res1 = ra::any(filtered1, [](const MyMovable& el) {
        return 41 == el.get_val();
    });

    (void) res1; slassert(!res1);
    
    auto filtered2 = ra::filter(std::move(rvec), [](const MyMovable& el) {
        return el.get_val() <= 41;
    }, ra::ignore_offcast<const MyMovable&>);
    auto transformed2 = ra::transform(std::move(filtered2), [](const MyMovable& el) {
        return MyMovableStr(sc::to_string(el.get_val()));
    });
    bool res2 = ra::any(transformed2, [](MyMovableStr& st) {
        return "41" == st.get_val();
    });
    
    (void) res2; slassert(res2);
    
    bool res3 = ra::any(rvec, [](const MyMovable& el) {
        return 41 == el.get_val();
    });
    
    (void) res3; slassert(res3);
}

void test_find() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    const auto vec2 = std::move(vec);

    auto rvec = ra::refwrap(vec2);
    auto filtered1 = ra::filter(std::move(rvec), [](const MyMovable& el) {
        return el.get_val() >= 42;
    }, ra::ignore_offcast<const MyMovable&>);
    auto transformed1 = ra::transform(std::move(filtered1), [](const MyMovable& el) {
        return MyMovableStr(sc::to_string(el.get_val()));
    });
    auto res1 = ra::find(transformed1, [](MyMovableStr& el) {
        return "41" == el.get_val();
    }, MyMovableStr("-1"));

    slassert("-1" == res1.get_val());

    auto filtered2 = ra::filter(std::move(rvec), [](const MyMovable & el) {
        return el.get_val() <= 41;
    }, ra::ignore_offcast<const MyMovable&>);
    auto transformed2 = ra::transform(std::move(filtered2), [](const MyMovable& el) {
        return MyMovableStr(sc::to_string(el.get_val()));
    });
    auto res2 = ra::find(transformed2, [](MyMovableStr& st) {
        return "41" == st.get_val();
    }, MyMovableStr("-1"));

    slassert("41" == res2.get_val());  
    
    auto mm = MyMovable(-1);
    auto res3 = ra::find(rvec, [](std::reference_wrapper<const MyMovable>& el) {
        return 41 == el.get().get_val();
    }, std::cref(mm));

    (void) res3;
    slassert(41 == res3.get().get_val());  
}

int main() {
    try {
        test_vector();
        test_range();
        test_emplace_to();
        test_any();
        test_find();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
