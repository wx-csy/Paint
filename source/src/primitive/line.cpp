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

#include <cmath>
#include <utility>
#include <stdexcept>

#include <paint/paint.h>
#include <paint/primitive.h>
#include <paint/util.h>
#include "algo.h"

using std::lround;          // from <cmath>
using std::abs; 
using std::swap;            // from <utility>
using util::limit_range;

static void DrawLine_DDA(Paint::ImageDevice& device, Paint::RGBColor color,
        float x1, float y1, float x2, float y2) {
    int ix1 = limit_range(x1, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iy1 = limit_range(y1, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        ix2 = limit_range(x2, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iy2 = limit_range(y2, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE);
    if (ix1 == ix2 && iy1 == iy2) {
        device.setPixel(ix1, iy1, color);
    } else if (abs(ix1 - ix2) > abs(iy1 - iy2)) {
        if (ix1 > ix2) { swap(ix1, ix2); swap(iy1, iy2); };
        float slope = (y2 - y1) / (x2 - x1), y = iy1;
        for (int x = ix1; x <= ix2; x++, y += slope) 
            device.setPixel(x, lround(y), color);
    } else {
        if (iy1 > iy2) { swap(ix1, ix2); swap(iy1, iy2); };
        float slope = (x2 - x1) / (y2 - y1), x = ix1;
        for (int y = iy1; y <= iy2; y++, x += slope) 
            device.setPixel(lround(x), y, color);
    }
}

static void DrawLine_Bresenham(Paint::ImageDevice& device, Paint::RGBColor color,
        float x1, float y1, float x2, float y2) {
    int ix1 = limit_range(x1, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iy1 = limit_range(y1, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        ix2 = limit_range(x2, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iy2 = limit_range(y2, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE);
    if (ix1 == ix2 && iy1 == iy2) {
        device.setPixel(ix1, iy1, color);
    } else if (abs(ix1 - ix2) > abs(iy1 - iy2)) {
        bool negate = false;
        if (ix1 > ix2) { swap(ix1, ix2); swap(iy1, iy2); }
        if (iy1 > iy2) { iy1 = -iy1; iy2 = -iy2; negate = true; }
        int dx = ix2 - ix1, dy = iy2 - iy1;
        long long f = -dx;
        device.setPixel(ix1, negate ? -iy1 : iy1, color);
        for (int x = ix1, y = iy1; x < ix2; x++) {
            if (f >= 0) { y++; f -= dx; } else { f += dx; }
            device.setPixel(x, negate ? -y : y, color);
            f += dy + dy - dx;
        }
    } else {
        bool negate = false;
        if (iy1 > iy2) { swap(ix1, ix2); swap(iy1, iy2); }
        if (ix1 > ix2) { ix1 = -ix1; ix2 = -ix2; negate = true; }
        int dy = iy2 - iy1, dx = ix2 - ix1;
        long long f = -dx;
        device.setPixel(negate ? -ix1 : ix1, iy1, color);
        for (int y = iy1, x = ix1; y < iy2; y++) { 
            if (f >= 0) { x++; f -= dy; } else { f += dy; }
            device.setPixel(negate ? -x : x, y, color);
            f += dx + dx - dy;
        }
    }
}

namespace Paint {
    //
    // class Line : public Element
    //
    void Line::paint(ImageDevice& device) {
        switch (algo) {
        case Algorithm::DDA :
            DrawLine_DDA(device, color, p1.x, p1.y, p2.x, p2.y);
            break;
        case Algorithm::Bresenham :
            DrawLine_Bresenham(device, color, p1.x, p1.y, p2.x, p2.y);
            break;
        default:
            throw std::invalid_argument("unknown algorithm"); 
        }
    }
    
    void Line::rotate(float x, float y, float rdeg) {
        float mat[2][2];
        init_rotate_matrix(rdeg, mat);
        std::tie(p1.x, p1.y) = rel_mat_apply(x, y, p1.x, p1.y, mat);
        std::tie(p2.x, p2.y) = rel_mat_apply(x, y, p2.x, p2.y, mat);
    }
    
    void Line::scale(float x, float y, float s) {
        std::tie(p1.x, p1.y) = rel_scale(x, y, p1.x, p1.y, s);
        std::tie(p2.x, p2.y) = rel_scale(x, y, p2.x, p2.y, s);
    }

    //
    // class Polygon : public Element
    //
    void Polygon::paint(ImageDevice& device) {
        switch (algo) {
        case Line::Algorithm::DDA :
            for (size_t i = 1; i < points.size(); i++)
                DrawLine_DDA(device, color,
                    points[i-1].first, points[i-1].second, 
                    points[i].first, points[i].second);
            if (points.size() > 2) 
                DrawLine_DDA(device, color,
                    points.back().first, points.back().second,
                    points.front().first, points.front().second);
            break;
        case Line::Algorithm::Bresenham :
            for (size_t i = 1; i < points.size(); i++)
                DrawLine_Bresenham(device, color,
                    points[i-1].first, points[i-1].second, 
                    points[i].first, points[i].second);
            if (points.size() > 2) 
                DrawLine_Bresenham(device, color,
                    points.back().first, points.back().second,
                    points.front().first, points.front().second);
            break;
        default:
            throw std::invalid_argument("unknown algorithm");
        }
    }
    
    void Polygon::rotate(float x, float y, float rdeg) {
        float mat[2][2];
        init_rotate_matrix(rdeg, mat);
        for (auto& p : points) 
            p = rel_mat_apply(x, y, p.first, p.second, mat);
    }

    void Polygon::scale(float x, float y, float s) {
        for (auto& p : points)
            p = rel_scale(x, y, p.first, p.second, s);
    }
}
