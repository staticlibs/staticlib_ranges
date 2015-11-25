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
#define	STATICLIB_RANGES_TEST_DOMAIN_CLASSES_HPP

#include <sstream>
#include <string>

class MyInt {
    int val;
public:
    MyInt(int val) : val(val) { }
    int get_int() const {
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

    std::string get_str() const {
        return val;
    }
    MyStr(const MyStr&) = delete;
    MyStr& operator=(const MyStr&) = delete;
    MyStr(MyStr&&) = delete;
    MyStr& operator=(MyStr&&) = delete;
};

class MyMovable {
    int val;
public:

    MyMovable(int val) : val(val) { }
    int get_val() const {
        return val;
    }
    void set_val(int val) {
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

class MyMovableStr {
    std::string val;
public:

    MyMovableStr(std::string val) : val(val) { }

    std::string get_val() const {
        return val;
    }

    void set_val(std::string val) {
        this->val = val;
    }
    MyMovableStr(const MyMovableStr&) = delete;
    MyMovable& operator=(const MyMovableStr&) = delete;

    MyMovableStr(MyMovableStr&& other) :
    val(other.val) {
        other.val = "";
    };

    MyMovableStr& operator=(MyMovableStr&& other) {
        this->val = other.val;
        other.val = "";
        return *this;
    }

    MyMovableStr clone() const {
        return MyMovableStr(val);
    }
};

#endif	/* STATICLIB_RANGES_TEST_DOMAIN_CLASSES_HPP */

