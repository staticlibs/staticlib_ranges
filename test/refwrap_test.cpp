/* 
 * File:   refwrap_test.cpp
 * Author: alex
 *
 * Created on February 16, 2015, 11:10 AM
 */

#include <iostream>
#include <cassert>
#include <string>
#include <memory>
#include <vector>

#include "staticlib/ranges.hpp"
#include "staticlib/ranges/refwrap.hpp"

namespace { //anonymous

namespace sit = staticlib::ranges;

class MyMovable {
    int val;
public:

    MyMovable(int val) : val(val) { }

    int get_int() const {
        return val;
    }
    void set_int(int val) {
        this->val = val;
    }
    
    
    MyMovable(const MyMovable&) = delete;
    MyMovable& operator=(const MyMovable&) = delete;

    MyMovable(MyMovable&& other) :
    val(other.val) {
        other.val = -1;
    };

    MyMovable& operator=(MyMovable&& other) {
        this->val = other.val;
        other.val = -1;
        return *this;
    }

    MyMovable clone() const {
        return MyMovable(val);
    }
};

void test_state_after_move() {
    auto a = MyMovable(42);
    auto b = std::move(a);
    assert(42 == b.get_int());
    assert(-1 == a.get_int());
}

void test_transform_refwrapped() {
    std::vector<MyMovable> vec{};
    vec.push_back(MyMovable(41));
    vec.push_back(MyMovable(42));
    vec.push_back(MyMovable(43));

    auto refwrapped = sit::refwrap(vec);
    auto transformed = sit::transform(refwrapped, [](MyMovable & el) {
        el.set_int(el.get_int() + 10);
        return std::ref(el);
    });
    auto filtered = sit::filter(transformed, [](MyMovable & el) {
        return 52 != el.get_int();
    }, sit::ignore_offcast<MyMovable&>);
    auto transformed2 = sit::transform(filtered, [](MyMovable & el) {
        el.set_int(el.get_int() - 10);
        return std::ref(el);
    });

    int size = 0;
    int sum = 0;
    for (auto el : transformed2) {
        size += 1;
        sum += el.get().get_int();
    }

    assert(2 == size);
    assert(84 == sum);

    assert(3 == vec.size());
    assert(41 == vec[0].get_int());
    assert(52 == vec[1].get_int());
    assert(43 == vec[2].get_int());
}

void test_refwrap_to_value() {
    std::vector<MyMovable> vec{};
    vec.emplace_back(41);
    vec.emplace_back(42);
    vec.emplace_back(43);
    
    auto refwrapped = sit::refwrap(vec);
    auto transformed = sit::transform(refwrapped, [](MyMovable& el) {
        return std::move(el);
    });
    
    std::vector<MyMovable> res = sit::emplace_to_vector(transformed);
    assert(3 == res.size());
    assert(41 == res[0].get_int());
    assert(42 == res[1].get_int());
    assert(43 == res[2].get_int());

    assert(3 == vec.size());
    assert(-1 == vec[0].get_int());
    assert(-1 == vec[1].get_int());
    assert(-1 == vec[2].get_int());
}

} // namespace

int main() {
    test_state_after_move();
    test_transform_refwrapped();
    test_refwrap_to_value();

    return 0;
}

