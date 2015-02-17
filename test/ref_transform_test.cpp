/* 
 * File:   ref_transform_test.cpp
 * Author: alex
 *
 * Created on February 17, 2015, 11:06 PM
 */

#include <cassert>
#include <vector>
#include <list>
#include <memory>
#include <iostream>

#include "domain_classes.hpp"
#include "staticlib/ranges.hpp"

namespace { // anonymous

namespace rf = staticlib::ranges::ref;

void test_return_ref() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);
    
    auto transformed = rf::transform(vec, [](MyMovable& el) -> MyMovable& {
        el.set_val(el.get_val() + 10);
        return el;
    });
    
    std::vector<int> res{};
    for (auto& el : transformed) {
        res.push_back(el.get_val());
    }

    assert(3 == res.size());
    assert(51 == res[0]);
    assert(52 == res[1]);
    assert(53 == res[2]);

    assert(3 == vec.size());
    assert(51 == vec[0].get_val());
    assert(52 == vec[1].get_val());
    assert(53 == vec[2].get_val());
}

void test_return_value() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);

    auto transformed = rf::transform_to_val(vec, [](MyMovable& el) {
        el.set_val(el.get_val() + 10);
        return std::move(el);
    });

    std::vector<int> res{};
    for (auto&& el : transformed) {
        res.push_back(el.get_val());
    }

    assert(3 == res.size());
    assert(51 == res[0]);
    assert(52 == res[1]);
    assert(53 == res[2]);

    assert(3 == vec.size());
    assert(-1 == vec[0].get_val());
    assert(-1 == vec[1].get_val());
    assert(-1 == vec[2].get_val());
}

void test_pipeline() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);

    auto transformed1 = rf::transform(vec, [](MyMovable& el) -> MyMovable& {
        el.set_val(el.get_val() + 10);
        return el;
    });

    auto transformed2 = rf::transform(transformed1, [](MyMovable& el) -> MyMovable& {
        el.set_val(el.get_val() - 20);
        return el;
    });
    
    auto transformed3 = rf::transform_to_val(transformed2, [](MyMovable& el) {
        return MyMovableStr(to_string(el.get_val()));
    });

    std::vector<std::string> res{};
    for (auto&& el : transformed3) {
        res.push_back(el.get_val());
    }

    assert(3 == res.size());
    assert("31" == res[0]);
    assert("32" == res[1]);
    assert("33" == res[2]);

    assert(3 == vec.size());
    assert(31 == vec[0].get_val());
    assert(32 == vec[1].get_val());
    assert(33 == vec[2].get_val());
}


} // namespace

int main() {
    test_return_ref();
    test_return_value();
    test_pipeline();

    return 0;
}

