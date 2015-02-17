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

#include "staticlib/ranges.hpp"

namespace { // anonymous

namespace mv = staticlib::ranges::move;

template<typename T>
std::string to_string(T t) {
    std::stringstream ss{};
    ss << t;
    return ss.str();
}

class MyInt {
    int val;
public:
    MyInt(int val) : val(val) { }
    int get_int() {
        return val;
    }
    MyInt(const MyInt&) = delete;
    MyInt& operator=(const MyInt&) = delete;
    MyInt(MyInt&&) = delete;
    MyInt& operator=(MyInt&&) = delete;
};

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
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    
    auto range = mv::transform(vec, [](std::unique_ptr<MyInt> el) {
        return std::unique_ptr<MyStr>(new MyStr(to_string(el->get_int() - 10)));
    });

    auto res = mv::emplace_to_vector(range);

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
    
    auto range1 = mv::transform(vec1, [](std::unique_ptr<MyInt> el) {
        return std::unique_ptr<MyStr>(new MyStr(to_string(el->get_int() + 10)));
    });
    auto range2 = mv::concat(range1, vec2);
    auto range3 = mv::filter(range2, [](std::unique_ptr<MyStr>& el) {
        return "52" != el->get_str();
    }, mv::ignore_offcast<std::unique_ptr<MyStr>>);
    auto range4 = mv::transform(range3, [](std::unique_ptr<MyStr> el) {
        return std::unique_ptr<MyStr>(new MyStr(el->get_str() + "_42"));
    });
    auto res = mv::emplace_to_vector(range4);

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

