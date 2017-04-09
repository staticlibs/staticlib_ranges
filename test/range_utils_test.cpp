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
#include "staticlib/support.hpp"

#include "staticlib/ranges/filter.hpp"
#include "staticlib/ranges/refwrap.hpp"
#include "staticlib/ranges/transform.hpp"

#include "domain_classes.hpp"

void test_vector() {
    auto vec = std::vector<std::unique_ptr<my_str>>{};
    vec.emplace_back(new my_str("foo"));
    vec.emplace_back(new my_str("bar"));
    
    auto res = sl::ranges::emplace_to_vector(std::move(vec));

    slassert(2 == res.size());
    slassert("foo" == res[0]->get_str());
    slassert("bar" == res[1]->get_str());
}

void test_range() {
    auto vec = std::vector<std::unique_ptr<my_str>>{};
    vec.emplace_back(new my_str("foo"));
    vec.emplace_back(new my_str("bar"));    
    vec.emplace_back(new my_str("baz"));
    auto range = sl::ranges::filter(std::move(vec), [](std::unique_ptr<my_str>& el) {
        return "bar" != el->get_str();
    }, sl::ranges::ignore_offcast<std::unique_ptr<my_str>>);
    
    auto res = sl::ranges::emplace_to_vector(std::move(range));
    
    slassert(2 == res.size());
    slassert("foo" == res[0]->get_str());
    slassert("baz" == res[1]->get_str());
}

void test_emplace_to() {
    auto vec = std::vector<std::unique_ptr<my_str>>{};
    vec.emplace_back(new my_str("foo"));
    vec.emplace_back(new my_str("bar"));

    auto range = sl::ranges::transform(std::move(vec), [](std::unique_ptr<my_str> el) {
        return std::unique_ptr<my_str>(new my_str(el->get_str() + "_42"));
    });

    auto res = std::vector<std::unique_ptr<my_str>>{};
    res.reserve(vec.size());
    sl::ranges::emplace_to(res, std::move(range));
    
    slassert(2 == res.size());
    slassert("foo_42" == res[0]->get_str());
    slassert("bar_42" == res[1]->get_str());
}

void test_any() {
    std::vector<my_movable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    const auto vec2 = std::move(vec);
    
    auto rvec = sl::ranges::refwrap(vec2);
    auto filtered1 = sl::ranges::filter(std::move(rvec), [](const my_movable& el) {
        return el.get_val() >= 42;
    }, sl::ranges::ignore_offcast<const my_movable&>);
    bool res1 = sl::ranges::any(filtered1, [](const my_movable& el) {
        return 41 == el.get_val();
    });

    (void) res1; slassert(!res1);
    
    auto filtered2 = sl::ranges::filter(std::move(rvec), [](const my_movable& el) {
        return el.get_val() <= 41;
    }, sl::ranges::ignore_offcast<const my_movable&>);
    auto transformed2 = sl::ranges::transform(std::move(filtered2), [](const my_movable& el) {
        return my_movable_str(sl::support::to_string(el.get_val()));
    });
    bool res2 = sl::ranges::any(transformed2, [](my_movable_str& st) {
        return "41" == st.get_val();
    });
    
    (void) res2; slassert(res2);
    
    bool res3 = sl::ranges::any(rvec, [](const my_movable& el) {
        return 41 == el.get_val();
    });
    
    (void) res3; slassert(res3);
}

void test_find() {
    std::vector<my_movable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    const auto vec2 = std::move(vec);

    auto rvec = sl::ranges::refwrap(vec2);
    auto filtered1 = sl::ranges::filter(std::move(rvec), [](const my_movable& el) {
        return el.get_val() >= 42;
    }, sl::ranges::ignore_offcast<const my_movable&>);
    auto transformed1 = sl::ranges::transform(std::move(filtered1), [](const my_movable& el) {
        return my_movable_str(sl::support::to_string(el.get_val()));
    });
    auto res1 = sl::ranges::find(transformed1, [](my_movable_str& el) {
        return "41" == el.get_val();
    }, my_movable_str("-1"));

    slassert("-1" == res1.get_val());

    auto filtered2 = sl::ranges::filter(std::move(rvec), [](const my_movable & el) {
        return el.get_val() <= 41;
    }, sl::ranges::ignore_offcast<const my_movable&>);
    auto transformed2 = sl::ranges::transform(std::move(filtered2), [](const my_movable& el) {
        return my_movable_str(sl::support::to_string(el.get_val()));
    });
    auto res2 = sl::ranges::find(transformed2, [](my_movable_str& st) {
        return "41" == st.get_val();
    }, my_movable_str("-1"));

    slassert("41" == res2.get_val());  
    
    auto mm = my_movable(-1);
    auto res3 = sl::ranges::find(rvec, [](std::reference_wrapper<const my_movable>& el) {
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
