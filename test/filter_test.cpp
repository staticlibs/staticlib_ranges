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

#include "staticlib/ranges.hpp"

namespace { //anonymous

namespace sit = staticlib::ranges;

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

void test_vector() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    vec.emplace_back(new MyInt(42));
    vec.emplace_back(new MyInt(43));
    
    auto offcasted = std::vector<std::unique_ptr<MyInt>>{};
    auto range = sit::filter(vec, [](std::unique_ptr<MyInt>& el) {
        return 42 == el->get_int();
    }, sit::offcast_into(offcasted));

    auto res = sit::emplace_to_vector(range);

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
    auto range = sit::concat(vec, list);
    
    auto filtered = sit::filter(range, [](std::unique_ptr<MyInt>& el) {
        return el->get_int() <= 40;
    }, sit::ignore_offcast<std::unique_ptr<MyInt>>);
    auto res = sit::emplace_to_vector(filtered);

    assert(1 == res.size());
    assert(40 == res[0]->get_int());
}


} // namespace
 
int main() {
    test_vector();
    test_range();

    return 0;
}

