/* 
 * File:   cppitertools_test.cpp
 * Author: alex
 *
 * Created on February 18, 2015, 8:56 AM
 */

#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <list>

#include "imap.hpp"
#include "filter.hpp"
#include "chain.hpp"

namespace { // anonymous

class MyMoveOnly {
    int val;
public:
    MyMoveOnly(int val) : val(val) { }
    MyMoveOnly(const MyMoveOnly&) = delete;
    MyMoveOnly& operator=(const MyMoveOnly&) = delete;

    MyMoveOnly(MyMoveOnly&& other) : val(other.val) { 
        other.val = -1;
    }

    MyMoveOnly& operator=(MyMoveOnly&& other) {
        this->val = other.val;
        other.val = -1;
        return *this;
    }

    int get_val() const {
        return val;
    }

    void set_val(int val) {
        this->val = val;
    }
};

void test_double_deref() {
    // source data
    std::vector<MyMoveOnly> vec{};
    vec.emplace_back(MyMoveOnly(41));
    vec.emplace_back(MyMoveOnly(42));
    vec.emplace_back(MyMoveOnly(43));
    // some transformations
    auto transformed1 = iter::imap([](MyMoveOnly & el) {
        int va = el.get_val();
        el.set_val(va + 10);
        return std::move(el);
    }, vec);
    // r-value ref is used here because l-value ref won't compile
    auto filtered = iter::filter([](const MyMoveOnly & el) {
        return 52 != el.get_val();
    }, transformed1);
    auto transformed2 = iter::imap([](MyMoveOnly & el) {
        int va = el.get_val();
        el.set_val(va - 10);
        return std::move(el);
    }, filtered);
    // print data
    for (auto&& el : transformed2) {
        std::cout << el.get_val() << std::endl;
    }
}

    void test_transform_vector() {
        std::vector<MyMoveOnly> vec{};
        vec.emplace_back(MyMoveOnly(41));
        vec.emplace_back(MyMoveOnly(42));
        vec.emplace_back(MyMoveOnly(43));

        auto transformed1 = iter::imap([](MyMoveOnly& el) {
            el.set_val(el.get_val() + 10);
            return std::move(el);
        }, vec);
        auto transformed2 = iter::imap([](MyMoveOnly& el) -> MyMoveOnly& {
            el.set_val(el.get_val() - 10);
            // this should not compile, 
            // returning temporary created by
            // previous lambda by reference here
            return el;
        }, transformed1);
        auto filtered = iter::filter([](MyMoveOnly& el) {
            return 42 != el.get_val();
        }, transformed2);

        for (auto&& el : filtered) {
            std::cout << el.get_val() << std::endl;
        }
        std::cout << std::endl;
        for (auto&& el : vec) {
            std::cout << el.get_val() << std::endl;
        }
    }

} // namespace

int main() {
    (void) test_double_deref;
    test_transform_vector();
    
    
    return 0;
}

