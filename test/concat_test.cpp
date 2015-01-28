/* 
 * File:   concat_test.cpp
 * Author: alex
 *
 * Created on January 28, 2015, 8:34 PM
 */

#include <cassert>
#include <vector>
#include <list>
#include <memory>
#include <iostream>

#include "staticlib/iterators.hpp"
#include "staticlib/iterators/transform.hpp"

namespace { // anonymous

namespace sit = staticlib::iterators;

class MyInt {
    int val;

public:
    MyInt(int val) : val(val) { }
    int get_int() { return val; }
    MyInt(const MyInt&) = delete;
    MyInt& operator=(const MyInt&) = delete;
    MyInt(MyInt&&) = delete;
    MyInt& operator=(MyInt&&) = delete;
};

void test_containers() {
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));

    auto list = std::list<std::unique_ptr<MyInt>>{};
    list.emplace_back(new MyInt(42));
    list.emplace_back(new MyInt(43));
    
    auto concatted = sit::concat(vec, list);
    auto res = sit::emplace_to_vector(concatted);
    
    assert(4 == res.size());
    assert(40 == res[0]->get_int());
    assert(41 == res[1]->get_int());
    assert(42 == res[2]->get_int());
    assert(43 == res[3]->get_int());
}

void test_empty_first() {
    auto vec_empty = std::vector<std::unique_ptr<MyInt>>{};
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));
    
    auto range = sit::concat(vec_empty, vec);
    auto res = sit::emplace_to_vector(range);
    
    assert(2 == res.size());
    assert(40 == res[0]->get_int());
    assert(41 == res[1]->get_int());
}

void test_empty_second() {
    auto vec_empty = std::vector<std::unique_ptr<MyInt>>{};
    auto vec = std::vector<std::unique_ptr<MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));

    auto range = sit::concat(vec, vec_empty);
    auto res = sit::emplace_to_vector(range);
    
    assert(2 == res.size());
    assert(40 == res[0]->get_int());
    assert(41 == res[1]->get_int());
}

void test_empty_both() {
    auto vec_empty1 = std::vector<std::unique_ptr<MyInt>>{};
    auto vec_empty2 = std::vector<std::unique_ptr<MyInt>>{};

    auto range = sit::concat(vec_empty1, vec_empty2);
    auto res = sit::emplace_to_vector(range);
    
    assert(0 == res.size());
}

void test_transformed() {
    auto vec = std::vector<std::unique_ptr < MyInt>>{};
    vec.emplace_back(new MyInt(40));
    vec.emplace_back(new MyInt(41));

    auto list = std::list<std::unique_ptr < MyInt>>{};
    list.emplace_back(new MyInt(42));
    list.emplace_back(new MyInt(43));
    list.emplace_back(new MyInt(44));
    
    auto transformed = sit::transform(vec, [](std::unique_ptr<MyInt> el) {
        return std::unique_ptr<MyInt>(new MyInt(el->get_int() - 10));
    });
    auto filtered = sit::filter(list, [](std::unique_ptr<MyInt>& el) {
        return 42 != el->get_int();
    }, [](std::unique_ptr<MyInt> el){ (void) el; });
    auto concatted = sit::concat(transformed, filtered);
    auto res = sit::emplace_to_vector(concatted);

    assert(4 == res.size());
    assert(30 == res[0]->get_int());
    assert(31 == res[1]->get_int());
    assert(43 == res[2]->get_int());
    assert(44 == res[3]->get_int());
}

} // namespace

int main() {
    test_containers();
    test_empty_first();
    test_empty_second();
    test_empty_both();
    test_transformed();
    
    return 0;
}

