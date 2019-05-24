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

#ifndef __PAINT_H__
#define __PAINT_H__

#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <utility>

namespace Paint {
    constexpr int MIN_COORDINATE = -8192, MAX_COORDINATE = 8192; 
    struct RGBColor { 
        uint8_t red, green, blue;

        explicit constexpr RGBColor(uint8_t red = 0,
                 uint8_t green = 0, 
                 uint8_t blue = 0) noexcept :
            red(red), green(green), blue(blue) { }
    };

    template <typename T>
    struct Point {
        T x, y;
        explicit constexpr Point(T x = T(), T y = T()) noexcept : x(x), y(y) {}
        Point operator + (Point rhs) const { return Point(x + rhs.x, y + rhs.y); }
        Point operator - (Point rhs) const { return Point(x - rhs.x, y - rhs.y); }
        Point operator - () const { return Point(-x, -y); }
        Point operator * (T k) const { return Point(x * k, y * k); }
        friend Point operator * (T k, Point pt) { return Point(k * pt.x, k * pt.y); }
        Point& operator += (Point rhs) { x += rhs.x; y += rhs.y; return *this; }
        Point& operator -= (Point rhs) { x -= rhs.x; y -= rhs.y; return *this; }
        Point& operator *= (T k) { x *= k; y *= k; return *this; }
        T lmax() { return std::max(std::abs(x), std::abs(y)); }
    };

    typedef Point<int> PointI;
    typedef Point<float> PointF;

    inline PointI pf2pi(PointF pt) { return PointI(std::lround(pt.x), std::lround(pt.y)); }

    namespace Colors {
        constexpr RGBColor black(0, 0, 0);
        constexpr RGBColor white(255, 255, 255); 
    }
        

}

#endif
