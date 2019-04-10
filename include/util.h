#ifndef __UTIL_H__
#define __UTIL_H__

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
        static_assert(std::is_arithmetic<T_to>::value &&
                      std::is_arithmetic<T_to>::value,
                      "template argument must be arithmetic type");
        typedef typename cond_type<
            std::is_floating_point<T_from>::value ||
            std::is_floating_point<T_to>::value, 
            double, long long>::type T_cast;

        if (T_cast(val) < T_cast(lower) || 
            T_cast(val) > T_cast(upper))
            throw std::range_error("value out of range");
        return val;
    }

    static inline std::vector<std::string> split(std::string str) {
        std::istringstream iss(str);
        return std::vector<std::string>(
            std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>()
        );
    }

}

#endif
