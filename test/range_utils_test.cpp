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

#include <cassert>
#include <vector>
#include <string>
#include <memory>

#include "domain_classes.hpp"
#include "staticlib/ranges.hpp"

namespace { //anonymous

namespace sit = staticlib::ranges;

class MyStr {
    std::string val;
public:
    MyStr(std::string val) : val(val) { }
    std::string get_str() {
        return val;
    }
    MyStr(const MyStr&) = delete;
    MyStr& operator=(const MyStr&) = delete;
    MyStr(MyStr&&) = delete;
    MyStr& operator=(MyStr&&) = delete;
};


void test_vector() {
    auto vec = std::vector<std::unique_ptr<MyStr>>{};
    vec.emplace_back(new MyStr("foo"));
    vec.emplace_back(new MyStr("bar"));
    
    auto res = sit::emplace_to_vector(vec);

    assert(2 == res.size());
    assert("foo" == res[0]->get_str());
    assert("bar" == res[1]->get_str());
}

void test_range() {
    auto vec = std::vector<std::unique_ptr<MyStr>>{};
    vec.emplace_back(new MyStr("foo"));
    vec.emplace_back(new MyStr("bar"));    
    vec.emplace_back(new MyStr("baz"));
    auto range = sit::filter(vec, [](std::unique_ptr<MyStr>& el) {
        return "bar" != el->get_str();
    }, sit::ignore_offcast<std::unique_ptr<MyStr>>);
    
    auto res = sit::emplace_to_vector(range);
    
    assert(2 == res.size());
    assert("foo" == res[0]->get_str());
    assert("baz" == res[1]->get_str());
}

void test_emplace_to() {
    auto vec = std::vector<std::unique_ptr<MyStr>>{};
    vec.emplace_back(new MyStr("foo"));
    vec.emplace_back(new MyStr("bar"));

    auto range = sit::transform(vec, [](std::unique_ptr<MyStr> el) {
        return std::unique_ptr<MyStr>(new MyStr(el->get_str() + "_42"));
    });

    auto res = std::vector<std::unique_ptr<MyStr>>{};
    res.reserve(vec.size());
    sit::emplace_to(res, range);
    
    assert(2 == res.size());
    assert("foo_42" == res[0]->get_str());
    assert("bar_42" == res[1]->get_str());
}

void test_any() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    const auto vec2 = std::move(vec);
    
    auto rvec = sit::refwrap(vec2);
    auto filtered1 = sit::filter(rvec, [](const MyMovable& el) {
        return el.get_val() >= 42;
    }, sit::ignore_offcast<const MyMovable&>);
    bool res1 = sit::any(filtered1, [](const MyMovable& el) {
        return 41 == el.get_val();
    });

    (void) res1; assert(!res1);
    
    auto filtered2 = sit::filter(rvec, [](const MyMovable& el) {
        return el.get_val() <= 41;
    }, sit::ignore_offcast<const MyMovable&>);
    auto transformed2 = sit::transform(filtered2, [](const MyMovable& el) {
        return MyMovableStr(to_string(el.get_val()));
    });
    bool res2 = sit::any(transformed2, [](MyMovableStr& st) {
        return "41" == st.get_val();
    });
    
    (void) res2; assert(res2);
    
    bool res3 = sit::any(rvec, [](const MyMovable& el) {
        return 41 == el.get_val();
    });
    
    (void) res3; assert(res3);
}

void test_find() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    const auto vec2 = std::move(vec);

    auto rvec = sit::refwrap(vec2);
    auto filtered1 = sit::filter(rvec, [](const MyMovable& el) {
        return el.get_val() >= 42;
    }, sit::ignore_offcast<const MyMovable&>);
    auto transformed1 = sit::transform(filtered1, [](const MyMovable& el) {
        return MyMovableStr(to_string(el.get_val()));
    });
    auto res1 = sit::find(transformed1, [](MyMovableStr& el) {
        return "41" == el.get_val();
    }, MyMovableStr("-1"));

    assert("-1" == res1.get_val());

    auto filtered2 = sit::filter(rvec, [](const MyMovable & el) {
        return el.get_val() <= 41;
    }, sit::ignore_offcast<const MyMovable&>);
    auto transformed2 = sit::transform(filtered2, [](const MyMovable& el) {
        return MyMovableStr(to_string(el.get_val()));
    });
    auto res2 = sit::find(transformed2, [](MyMovableStr& st) {
        return "41" == st.get_val();
    }, MyMovableStr("-1"));

    assert("41" == res2.get_val());  
    
    auto mm = MyMovable(-1);
    auto res3 = sit::find(rvec, [](std::reference_wrapper<const MyMovable>& el) {
        return 41 == el.get().get_val();
    }, std::cref(mm));

    (void) res3;
    assert(41 == res3.get().get_val());  
}

} // namespace

int main() {
    test_vector();
    test_range();
    test_emplace_to();
    test_any();
    test_find();
    
    return 0;
}

