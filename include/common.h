#ifndef __COMMON_H__

#include <string>
#include <type_traits>
#include <limits>
#include <stdexcept>

// run paint in batch mode
void batch();

template <typename T = signed>
static inline T str2int(const std::string& str) {
    static_assert(std::is_signed<T>::value, 
        "template argument must be signed integer");
    std::size_t pos;
    long long res = stoll(str, &pos);
    if (pos < str.size())
        throw std::invalid_argument("invalid signed integer '" + str + "'");
    if (res < std::numeric_limits<T>::min() || 
        res > std::numeric_limits<T>::max())
        throw std::out_of_range("integer '" + str + "' out of range");
    return res;
}

template <typename T = unsigned>
static inline T str2uint(const std::string& str) {
    static_assert(std::is_unsigned<T>::value, 
        "template argument must be unsigned integer");
    std::size_t pos;
    unsigned long long res = stoull(str, &pos);
    if (pos < str.size())
        throw std::invalid_argument("invalid unsigned integer '" + str + "'");
    if (res < std::numeric_limits<T>::min() || 
        res > std::numeric_limits<T>::max())
        throw std::out_of_range("integer '" + str + "' out of range");
    return res;
}

template <typename T = float>
static inline T str2float(const std::string& str) {
    static_assert(std::is_floating_point<T>::value, 
        "template argument must be floating point");
    std::size_t pos;
    double res = stod(str, &pos);
    if (pos < str.size())
        throw std::invalid_argument("invalid float '" + str + "'");
    return res;
}

#endif
