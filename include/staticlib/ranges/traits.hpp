/*
 * Copyright 2017, alex at staticlibs.net
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
 * File:   traits.hpp
 * Author: alex
 *
 * Created on February 3, 2017, 10:01 PM
 */

#ifndef STATICLIB_RANGES_TRAITS_HPP
#define STATICLIB_RANGES_TRAITS_HPP

#include <functional>

namespace staticlib {
namespace ranges {

/**
 * Type trait to detect `std::reference_wrapper`, negative case
 */
template<typename T>
struct is_reference_wrapper {
    static const bool value = false;
};

/**
 * Type trait to detect `std::reference_wrapper`, positive case
 */
template<typename T>
struct is_reference_wrapper<std::reference_wrapper<T>> {
    static const bool value = true;
};

} // namespace
}

#endif /* STATICLIB_RANGES_TRAITS_HPP */

