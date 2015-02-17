/* 
 * File:   move_utils_test.cpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:35 PM
 */

#include <cassert>
#include <vector>
#include <string>
#include <memory>

#include "staticlib/ranges/move/concat.hpp"
#include "staticlib/ranges/move/filter.hpp"
#include "staticlib/ranges/move/transform.hpp"
#include "staticlib/ranges/move/utils.hpp"


namespace { //anonymous

namespace mv = staticlib::ranges::move;

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
    
    auto res = mv::emplace_to_vector(vec);

    assert(2 == res.size());
    assert("foo" == res[0]->get_str());
    assert("bar" == res[1]->get_str());
}

void test_range() {
    auto vec = std::vector<std::unique_ptr<MyStr>>{};
    vec.emplace_back(new MyStr("foo"));
    vec.emplace_back(new MyStr("bar"));    
    vec.emplace_back(new MyStr("baz"));
    auto range = mv::filter(vec, [](std::unique_ptr<MyStr>& el) {
        return "bar" != el->get_str();
    }, mv::ignore_offcast<std::unique_ptr<MyStr>>);
    
    auto res = mv::emplace_to_vector(range);
    
    assert(2 == res.size());
    assert("foo" == res[0]->get_str());
    assert("baz" == res[1]->get_str());
}

void test_emplace_to() {
    auto vec = std::vector<std::unique_ptr<MyStr>>{};
    vec.emplace_back(new MyStr("foo"));
    vec.emplace_back(new MyStr("bar"));

    auto range = mv::transform(vec, [](std::unique_ptr<MyStr> el) {
        return std::unique_ptr<MyStr>(new MyStr(el->get_str() + "_42"));
    });

    auto res = std::vector<std::unique_ptr<MyStr>>{};
    res.reserve(vec.size());
    mv::emplace_to(res, range);
    
    assert(2 == res.size());
    assert("foo_42" == res[0]->get_str());
    assert("bar_42" == res[1]->get_str());
}

} // namespace

int main() {
    test_vector();
    test_range();
    test_emplace_to();
    
    return 0;
}

