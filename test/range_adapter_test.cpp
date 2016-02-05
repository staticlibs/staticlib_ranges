/*
 * Copyright 2015, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   range_adapter_test.cpp
 * Author: alex
 *
 * Created on November 26, 2015, 11:15 AM
 */

#include "staticlib/ranges/range_adapter.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"

#include "staticlib/ranges/range_utils.hpp"
#include "staticlib/ranges/transform.hpp"

#include "domain_classes.hpp"

namespace ra = staticlib::ranges;

class MyMovableRange : public ra::range_adapter<MyMovableRange, MyMovable> {
    const int max;
    int count = 0;
    
public:
    MyMovableRange(int max) :
    max(max) { }

    MyMovableRange(MyMovableRange&& other) :
    max(other.max),
    count(other.count) { }

public:
    bool compute_next() {
        if (count < max) {
            count += 1;
            return this->set_current(MyMovable{count});
        } else {
            return false;
        }
    }
    
};

void test_movable() {
    MyMovableRange range{3};
    auto transformed = ra::transform(std::move(range), [](MyMovable el) {
        return el.get_val();
    });
    auto vec = ra::emplace_to_vector(std::move(transformed));
    slassert(3 == vec.size());
    slassert(1 == vec[0]);
    slassert(2 == vec[1]);
    slassert(3 == vec[2]);
}

int main() {
    try {
        test_movable();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
