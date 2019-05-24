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

#include <cassert>
#include <vector>
#include <utility>
#include <stdexcept>
#include "paint.h"
#include "util.h"

#ifndef __GNUC__
template <typename T>
int __builtin_ctz(T val) {
    if (val == 0) {
        return sizeof(T) * 8;
    } else {
        int ans = 0;
        while (!(val & 1)) {
            val >>= 1;
            ans++;
        }
        return ans;
    }
}
#endif

enum {
    XMIN_LEFT = 0,
    XMAX_RIGHT = 1,
    YMIN_DOWN = 2,
    YMAX_UP = 3
};

static inline uint8_t cs_encode(Paint::PointF p, float xmin, float xmax, float ymin, float ymax) {
    return uint8_t(p.x < xmin) << XMIN_LEFT | uint8_t(p.x > xmax) << XMAX_RIGHT
         | uint8_t(p.y < ymin) << YMIN_DOWN | uint8_t(p.y > ymax) << YMAX_UP;
}

static inline Paint::PointF reg_x(Paint::PointF p1, Paint::PointF p2, float x) {
    float f = (x - p1.x) / (p2.x - p1.x);
    return Paint::PointF(x, f * p1.y + (1 - f) * p2.y);
}

static inline Paint::PointF reg_y(Paint::PointF p1, Paint::PointF p2, float y) {
    float f = (y - p1.y) / (p2.y - p1.y);
    return Paint::PointF(f * p1.x + (1 - f) * p2.x, y);
}

static std::pair<Paint::PointF, Paint::PointF>
        cohen_sutherland(Paint::PointF p1, Paint::PointF p2, float x1, float x2, float y1, float y2) {
    uint8_t c1 = cs_encode(p1, x1, x2, y1, y2);
    uint8_t c2 = cs_encode(p2, x1, x2, y1, y2);
    // the segment lies entirely in the region
    if (c1 == 0 and c2 == 0) return {p1, p2};
    // the segment lies entirely outside the region
    if (c1 & c2) throw std::range_error("segment lies outside the region");
    // let p1 be outside the region
    if (c1 == 0) { std::swap(p1, p2); std::swap(c1, c2); }
    switch (__builtin_ctz(c1)) {
    case XMIN_LEFT:     p1 = reg_x(p1, p2, x1); break;
    case XMAX_RIGHT:    p1 = reg_x(p1, p2, x2); break;
    case YMIN_DOWN:     p1 = reg_y(p1, p2, y1); break;
    case YMAX_UP:       p1 = reg_y(p1, p2, y2); break;
    default: assert(!"unexpected region code");
    }
    return cohen_sutherland(p1, p2, x1, x2, y1, y2);
}

static std::pair<Paint::PointF, Paint::PointF>
liang_barsky(Paint::PointF p1, Paint::PointF p2, float x1, float x2, float y1, float y2) {
    float p[4] = {p1.x - p2.x, p2.x - p1.x, p1.y - p2.y, p2.y - p1.y};
    float q[4] = {p1.x - x1, x2 - p1.x, p1.y - y1, y2 - p1.y};
    float u1 = 0.0, u2 = 1.0;
    for (int i = 0; i < 4; i++) {
        if (p[i] == 0) {
            if (q[i] < 0) throw std::range_error("segment lies outside the region");
            else continue;
        }
        float r = q[i] / p[i];
        if (p[i] < 0) u1 = std::max(u1, r);
        else if (p[i] > 0) u2 = std::min(u2, r);
    }
    if (u1 >= u2) throw std::range_error("segment lies outside the region");
    return {p1 + u1 * (p2 - p1), p1 + u2 * (p2 - p1)};
};

namespace Paint {
    //
    // class Line : public Element
    //
    void Line::clip(float x1, float y1, float x2, float y2,
                    LineClippingAlgorithm algo) {
        std::vector<Line> lines;
        switch (algo) {
        case LineClippingAlgorithm::CohenSutherland:
            std::tie(p1, p2) = cohen_sutherland(p1, p2, x1, x2, y1, y2);
            break;
        case LineClippingAlgorithm::LiangBarsky:
            std::tie(p1, p2) = liang_barsky(p1, p2, x1, x2, y1, y2);
            break;
        }
    }
}
