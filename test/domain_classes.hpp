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
 * File:   domain_classes.hpp
 * Author: alex
 *
 * Created on February 17, 2015, 11:04 PM
 */

#ifndef STATICLIB_RANGES_TEST_DOMAIN_CLASSES_HPP
#define STATICLIB_RANGES_TEST_DOMAIN_CLASSES_HPP

#include <sstream>
#include <string>

class my_int {
    int val;
public:
    my_int(int val) : val(val) { }
    int get_int() const {
        return val;
    }
    
    my_int(const my_int&) = delete;
    my_int& operator=(const my_int&) = delete;
    my_int(my_int&&) = delete;
    my_int& operator=(my_int&&) = delete;
};

class my_str {
    std::string val;
public:

    my_str(std::string val) : val(val) { }

    std::string get_str() const {
        return val;
    }
    my_str(const my_str&) = delete;
    my_str& operator=(const my_str&) = delete;
    my_str(my_str&&) = delete;
    my_str& operator=(my_str&&) = delete;
};

class my_movable {
    int val;
public:

    my_movable(int val) : val(val) { }
    int get_val() const {
        return val;
    }
    void set_val(int val) {
        this->val = val;
    }
    my_movable(const my_movable&) = delete;
    my_movable& operator=(const my_movable&) = delete;

    my_movable(my_movable&& other) :
    val(other.val) {
        other.val = -1;
    };

    my_movable& operator=(my_movable&& other) {
        this->val = other.val;
        other.val = -1;
        return *this;
    }

    my_movable clone() const {
        return my_movable(val);
    }
};

class my_movable_str {
    std::string val;
public:

    my_movable_str(std::string val) : val(val) { }

    std::string get_val() const {
        return val;
    }

    void set_val(std::string val) {
        this->val = val;
    }
    my_movable_str(const my_movable_str&) = delete;
    my_movable& operator=(const my_movable_str&) = delete;

    my_movable_str(my_movable_str&& other) :
    val(other.val) {
        other.val = "";
    };

    my_movable_str& operator=(my_movable_str&& other) {
        this->val = other.val;
        other.val = "";
        return *this;
    }

    my_movable_str clone() const {
        return my_movable_str(val);
    }
};

#endif /* STATICLIB_RANGES_TEST_DOMAIN_CLASSES_HPP */

