#ifndef __COMMON_H__

#include <string>
#include <type_traits>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <iostream>
// run paint in batch mode
void batch();

long long from_string(const std::string& str) {
    std::istringstream iss(str);
    long long ret;
    if ((iss << ret).fail() || iss.peek() != iss.eof())
        throw std::argument_error("invalid argument '" + str + "'");
    return ret;
}

template <typename T>
static inline T limit_range(T val, 
    T lower = std::numeric_limits<T>::min(),
    T upper = std::numeric_limits<T>::max()) {
    if (val < lower || val > upper)
        throw std::range_error("value out of range");
    return val;
}

#endif
