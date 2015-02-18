/* 
 * File:   transform_test.cpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:35 PM
 */

#include <cassert>
#include <vector>
#include <list>
#include <memory>
#include <sstream>
#include <iostream>

#include "domain_classes.hpp"
#include "staticlib/ranges.hpp"

namespace { // anonymous

namespace sit = staticlib::ranges;

void test_vector() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    
    auto range = sit::transform(vec, [](std::unique_ptr<MyInt> el) {
        return std::unique_ptr<MyStr>(new MyStr(to_string(el->get_int() - 10)));
    });

    auto res = sit::emplace_to_vector(range);

    assert(2 == res.size());
    assert("30" == res[0]->get_str());
    assert("31" == res[1]->get_str());
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
        return std::unique_ptr<MyStr>(new MyStr(to_string(el->get_int() + 10)));
    });
    auto range2 = sit::concat(range1, vec2);
    auto range3 = sit::filter(range2, [](std::unique_ptr<MyStr>& el) {
        return "52" != el->get_str();
    }, sit::ignore_offcast<std::unique_ptr<MyStr>>);
    auto range4 = sit::transform(range3, [](std::unique_ptr<MyStr> el) {
        return std::unique_ptr<MyStr>(new MyStr(el->get_str() + "_42"));
    });
    auto res = sit::emplace_to_vector(range4);

    assert(4 == res.size());
    assert("50_42" == res[0]->get_str());
    assert("51_42" == res[1]->get_str());
    assert("53_42" == res[2]->get_str());
    assert("54_42" == res[3]->get_str());
}

} // namespace


int main() {
    test_vector();
    test_range();

    return 0;
}

