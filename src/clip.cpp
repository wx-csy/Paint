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
    XMIN_LEFT = 1,
    XMAX_RIGHT = 2,
    YMIN_DOWN = 3,
    YMAX_UP = 4
};

static inline uint8_t cs_encode(Paint::PointF p, float xmin, float xmax, float ymin, float ymax) {
    return uint8_t(p.x < xmin) << XMIN_LEFT | uint8_t(p.x > xmax) << XMAX_RIGHT
         | uint8_t(p.y < ymin) << YMIN_DOWN | uint8_t(p.y > ymax) << YMAX_UP;
}

static inline std::vector<std::pair<Paint::PointF, Paint::PointF>>
        cohen_sutherland(Paint::PointF p1, Paint::PointF p2, float x1, float x2, float y1, float y2) {
    uint8_t c1 = cs_encode(p1, x1, x2, y1, y2);
    uint8_t c2 = cs_encode(p2, x1, x2, y1, y2);
    // the segment lies entirely in the region
    if (c1 == 0 and c2 == 0) return {{p1, p2}};
    // the segment lies entirely outside the region
    if (c1 & c2) return {};
    // let p1 be outside the region
    if (c1 == 0) { std::swap(p1, p2); std::swap(c1, c2); }
    switch (__builtin_ctz(c1)) {
    case XMIN_LEFT:     p1.x = x1; break;
    case XMAX_RIGHT:    p1.x = x2; break;
    case YMIN_DOWN:     p1.y = y1; break;
    case YMAX_UP:       p1.y = y2; break;
    default: assert(!"unexpected region code");
    }
    return cohen_sutherland(p1, p2, x1, x2, y1, y2);
}

namespace Paint {
    //
    // class Line : public Element
    //
    std::vector<Line> Line::clip(float x1, float y1, float x2, float y2,
                    LineClippingAlgorithm algo) const {
        std::vector<std::pair<PointF, PointF>> points;
        std::vector<Line> lines;
        switch (algo) {
        case LineClippingAlgorithm::CohenSutherland:
            points = cohen_sutherland(p1, p2, x1, x2, y1, y2);
            break;
        case LineClippingAlgorithm::LiangBarsky:
            throw std::runtime_error("not implemented");
        }
        lines.reserve(points.size());
        for (auto& p : points) lines.emplace_back(p.first, p.second, color, this->algo);
        return lines;
    }
}