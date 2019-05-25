/*
    Paint, a simple rasterization tool
    Copyright (C) 2019 Chen Shaoyuan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __UTIL_H__
#define __UTIL_H__

#include <cmath>
#include <string>
#include <iterator>
#include <type_traits>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <iostream>

namespace util {
    
    template <bool cond, typename type_if_true, typename type_if_false>
    struct cond_type;
    
    template <typename type_if_true, typename type_if_false>
    struct cond_type<true, type_if_true, type_if_false> {
        typedef type_if_true type;
    };
    
    template <typename type_if_true, typename type_if_false>
    struct cond_type<false, type_if_true, type_if_false> {
        typedef type_if_false type;
    };

    template <typename T = long long>
    T from_string(const std::string& str) {
        std::istringstream iss(str);
        T ret;
        if ((iss >> ret).fail() && (iss.peek() != iss.eof()))
            throw std::invalid_argument("invalid argument '" + str + "'");
        return ret;
    }

    template <typename T_to, typename T_from>
    static inline T_to limit_range(T_from val, 
        T_to lower = std::numeric_limits<T_to>::min(),
        T_to upper = std::numeric_limits<T_to>::max()) {
        static_assert(std::is_arithmetic<T_from>::value &&
                      std::is_arithmetic<T_to>::value,
                      "template arguments must be arithmetic type");
        typedef typename cond_type<
            std::is_floating_point<T_from>::value ||
            std::is_floating_point<T_to>::value, 
            double, long long>::type T_cast;

        if (T_cast(val) < T_cast(lower) || 
            T_cast(val) > T_cast(upper))
            throw std::range_error("value out of range");
        if (std::is_floating_point<T_from>::value && 
            std::is_integral<T_to>::value) {
            return std::round(val);
        } else {
            return val;
        }
    }

    template <typename T>
    static inline T clamp(T val,
            T lower = std::numeric_limits<T>::min(),
            T upper = std::numeric_limits<T>::max()) {
        if (val < lower) val = lower;
        if (val > upper) val = upper;
        return val;
    }

    static inline std::vector<std::string> split(const std::string &str) {
        std::istringstream iss(str);
        return std::vector<std::string>(
            std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>()
        );
    }

}

#endif
