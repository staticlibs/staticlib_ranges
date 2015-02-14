/* 
 * File:   clone_test.cpp
 * Author: alex
 *
 * Created on January 30, 2015, 11:55 AM
 */

#include <vector>
#include <cassert>
#include <string>
#include <memory>

#include "staticlib/ranges.hpp"

namespace { // anonymous

namespace sit = staticlib::ranges;

class MyMovable {
    int val;
public:
    MyMovable(int val) : val(val) { }
    int get_int() {
        return val;
    }
    MyMovable(const MyMovable&) = delete;
    MyMovable& operator=(const MyMovable&) = delete;    
    
    MyMovable(MyMovable&& other) : 
    val(other.val) { };
    
    MyMovable& operator=(MyMovable&& other) {
        this->val = other.val;
        return *this;
    }
    
    MyMovable clone() const {
        return MyMovable(val);
    }
};

void test_source_not_moved() {
    auto vec = std::vector<MyMovable>{};
    vec.emplace_back(MyMovable(40));
    vec.emplace_back(MyMovable(41));

    auto cloned = sit::clone(vec);
    auto res = sit::emplace_to_vector(cloned);

    assert(2 == res.size());
    assert(40 == res[0].get_int());
    assert(41 == res[1].get_int());

    assert(2 == vec.size());
    assert(40 == res[0].get_int());
    assert(41 == res[1].get_int());
}

// this is deliberately cannot be compiled, 
// as move-only ranges cannot be copied
//
//void test_range_source() {
//    auto vec = std::vector<MyMovable>{};
//    vec.emplace_back(MyMovable(40));
//    vec.emplace_back(MyMovable(41));
//    
//    auto transformed = sit::transform(vec, [](MyMovable el) {
//        return MyMovable(el.get_int() + 10);
//    });
//    
//    auto cloned = sit::clone(transformed);
//    auto res = sit::emplace_to_vector(copied);
//    
//    assert(2 == res.size());
//    assert(50 == res[0].get_int());
//    assert(51 == res[1].get_int());
//}

void test_range_wrapper() {
    auto vec = std::vector<MyMovable>{};
    vec.emplace_back(MyMovable(40));
    vec.emplace_back(MyMovable(41));

    auto cloned = sit::clone(vec);
    auto transformed = sit::transform(cloned, [](MyMovable el) {
        return MyMovable(el.get_int() + 10);
    });
    auto res = sit::emplace_to_vector(transformed);

    assert(2 == res.size());
    assert(50 == res[0].get_int());
    assert(51 == res[1].get_int());
}

} // namespace

int main() {
    test_source_not_moved();
//    test_range_source();
    test_range_wrapper();

    return 0;
}

