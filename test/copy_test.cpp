/* 
 * File:   copy_test.cpp
 * Author: alex
 *
 * Created on January 30, 2015, 11:26 AM
 */

#include <vector>
#include <cassert>
#include <string>

#include "staticlib/ranges.hpp"

namespace { // anonymous

namespace sit = staticlib::ranges;

void test_source_not_moved() {
    std::vector<std::string> vec{"foo", "bar"};
    
    auto copied = sit::copy(vec);
    auto res = sit::emplace_to_vector(copied);

    assert(2 == res.size());
    assert("foo" == res[0]);
    assert("bar" == res[1]);

    assert(2 == vec.size());
    assert("foo" == vec[0]);
    assert("bar" == vec[1]);
}

// this is deliberately cannot be compiled, 
// as move-only ranges cannot be copied
//
//void test_range_source() {
//    std::vector<std::string> vec{"foo", "bar"};
//    
//    auto transformed = sit::transform(vec, [](std::string el) {
//        return el + "_42";
//    });
//    
//    auto copied = sit::copy(transformed);
//    auto res = sit::emplace_to_vector(copied);
//    
//    assert(2 == res.size());
//    assert(50 == res[0]->get_int());
//    assert(51 == res[1]->get_int());
//}

void test_range_wrapper() {
    std::vector<std::string> vec{"foo", "bar"};

    auto copied = sit::copy(vec);
    auto transformed = sit::transform(copied, [](std::string el) {
        return el + "_42";
    });
    auto res = sit::emplace_to_vector(transformed);

    assert(2 == res.size());
    assert("foo_42" == res[0]);
    assert("bar_42" == res[1]);
}

} // namespace

int main() {
    test_source_not_moved();
//    test_range_source();
    test_range_wrapper();

    return 0;
}

