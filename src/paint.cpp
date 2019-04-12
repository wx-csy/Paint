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
#include "paint.h"
#include "util.h"
using std::lround;          // from <cmath>
using std::abs; 
using std::swap;            // from <utility>
using util::limit_range;

// degree is given clockwise
static inline void init_rotate_matrix(float deg, float mat[2][2]) {
    float rad = std::fmod(deg, 360.0f) / 180.0f * std::acos(-1.0f);
    mat[0][0] = std::cos(rad);  mat[0][1] = -std::sin(rad);
    mat[1][0] = -mat[0][1];     mat[1][1] = mat[0][0];
}

template <size_t N>
static inline void matrix_transform(float mat[N][N], float point[N]) {
    float res[N] = {};
    for (size_t i = 0; i < N; i++)
        for (size_t j = 0; j < N; j++)
            res[i] += mat[i][j] * point[j];
    for (size_t i = 0; i < N; i++)
        point[i] = res[i];
}

static inline std::pair<float, float> rel_mat_apply
        (float cx, float cy, float x, float y, float mat[2][2]) {
    std::cout << cx << ' ' << cy << ' ' << x << ' ' << y << std::endl;
    float point[2] = {x - cx, y - cy};
    matrix_transform(mat, point);
    std::cout << point[0] + cx << ' ' << point[1] + cy << std::endl;
    return std::make_pair(point[0] + cx, point[1] + cy);
}

static void DrawLine_DDA(Paint::Canvas& canvas, Paint::RGBColor color,
        float x1, float y1, float x2, float y2) {
    int ix1 = limit_range(x1, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iy1 = limit_range(y1, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        ix2 = limit_range(x2, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iy2 = limit_range(y2, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE);
    if (ix1 == ix2 && iy1 == iy2) {
        canvas.setPixel(ix1, iy1, color);
    } else if (abs(ix1 - ix2) > abs(iy1 - iy2)) {
        if (ix1 > ix2) { swap(ix1, ix2); swap(iy1, iy2); };
        float slope = (y2 - y1) / (x2 - x1), y = iy1;
        for (int x = ix1; x <= ix2; x++, y += slope) 
            canvas.setPixel(x, lround(y), color);
    } else {
        if (iy1 > iy2) { swap(ix1, ix2); swap(iy1, iy2); };
        float slope = (x2 - x1) / (y2 - y1), x = ix1;
        for (int y = iy1; y <= iy2; y++, x += slope) 
            canvas.setPixel(lround(x), y, color);
    }
}

static void DrawLine_Bresenham(Paint::Canvas& canvas, Paint::RGBColor color,
        float x1, float y1, float x2, float y2) {
    int ix1 = limit_range(x1, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iy1 = limit_range(y1, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        ix2 = limit_range(x2, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iy2 = limit_range(y2, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE);
    if (ix1 == ix2 && iy1 == iy2) {
        canvas.setPixel(ix1, iy1, color);
    } else if (abs(ix1 - ix2) > abs(iy1 - iy2)) {
        bool negate = false;
        if (ix1 > ix2) { swap(ix1, ix2); swap(iy1, iy2); }
        if (iy1 > iy2) { iy1 = -iy1; iy2 = -iy2; negate = true; }
        int dx = ix2 - ix1, dy = iy2 - iy1;
        long long f = -dx;
        canvas.setPixel(ix1, negate ? -iy1 : iy1, color);
        for (int x = ix1, y = iy1; x < ix2; x++) {
            if (f >= 0) { y++; f -= dx; } else { f += dx; }
            canvas.setPixel(x, negate ? -y : y, color);
            f += dy + dy - dx;
        }
    } else {
        bool negate = false;
        if (iy1 > iy2) { swap(ix1, ix2); swap(iy1, iy2); }
        if (ix1 > ix2) { ix1 = -ix1; ix2 = -ix2; negate = true; }
        int dy = iy2 - iy1, dx = ix2 - ix1;
        long long f = -dx;
        canvas.setPixel(negate ? -ix1 : ix1, iy1, color);
        for (int y = iy1, x = ix1; y < iy2; y++) { 
            if (f >= 0) { x++; f -= dy; } else { f += dy; }
            canvas.setPixel(negate ? -x : x, y, color);
            f += dx + dx - dy;
        }
    }
}

static void DrawEllipse_Midpoint(Paint::Canvas& canvas, Paint::RGBColor color,
        float x, float y, float rx, float ry) {
    long long
        ix = limit_range(x, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iy = limit_range(y, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        irx = limit_range(rx, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iry = limit_range(ry, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE),
        irx2 = irx * irx,
        iry2 = iry * iry;

    auto quaddraw = [&] (int cx, int cy) {
        canvas.setPixel(ix + cx, iy + cy, color);
        canvas.setPixel(ix - cx, iy + cy, color);
        canvas.setPixel(ix + cx, iy - cy, color);
        canvas.setPixel(ix - cx, iy - cy, color);
    };
    quaddraw(0, iry);
    quaddraw(irx, 0);
    
    for (int i = 0; i < 2; i++) {
        long long p = iry2 - irx2 * iry + irx2 / 4.0;
        long long px = 0, py = 2 * irx2 * iry;
        int cx = 0, cy = iry;
        while (px < py) {
            cx++;
            px += 2 * iry2;
            if (p < 0) {
                p += iry2 + px;
            } else {
                cy--;
                py -= 2 * irx2;
                p += iry2 + px - py;
            }
            if (i == 0) quaddraw(cx, cy); else quaddraw(cy, cx);
        }
        swap(irx, iry);
        swap(irx2, iry2);
    }
}

namespace Paint {
    //
    // class Line : public Element
    //
    void Line::paint(Canvas& canvas) {
        switch (algo) {
        case LineDrawingAlgorithm::DDA :
            DrawLine_DDA(canvas, color, x1, y1, x2, y2);
            break;
        case LineDrawingAlgorithm::Bresenham :
            DrawLine_Bresenham(canvas, color, x1, y1, x2, y2);
            break;
        default:
            throw std::invalid_argument("unknown algorithm"); 
        }
    }
    
    void Line::rotate(float x, float y, float rdeg) {
        float mat[2][2];
        init_rotate_matrix(rdeg, mat);
        std::tie(x1, y1) = rel_mat_apply(x, y, x1, y1, mat);
        std::tie(x2, y2) = rel_mat_apply(x, y, x2, y2, mat);
    }

    void Line::clip(float x1, float y1, float x2, float y2, 
            LineClippingAlgorithm algo) {
        throw std::runtime_error("not implemented");
    }

    //
    // class Polygon : public Element
    //
    void Polygon::paint(Canvas& canvas) {
        for (auto p : points) {
            std::cout << p.first << ' ' << p.second << std::endl;
        }
        switch (algo) {
        case LineDrawingAlgorithm::DDA :
            for (size_t i = 1; i < points.size(); i++)
                DrawLine_DDA(canvas, color, 
                    points[i-1].first, points[i-1].second, 
                    points[i].first, points[i].second);
            if (points.size() > 2) 
                DrawLine_DDA(canvas, color, 
                    points.back().first, points.back().second,
                    points.front().first, points.front().second);
            break;
        case LineDrawingAlgorithm::Bresenham :
            for (size_t i = 1; i < points.size(); i++)
                DrawLine_Bresenham(canvas, color, 
                    points[i-1].first, points[i-1].second, 
                    points[i].first, points[i].second);
            if (points.size() > 2) 
                DrawLine_Bresenham(canvas, color, 
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
            std::tie(p.first, p.second) = 
                rel_mat_apply(x, y, p.first, p.second, mat);
    }
    
    //
    // class Ellipse : public Element
    //
    void Ellipse::paint(Canvas& canvas) {
        DrawEllipse_Midpoint(canvas, color, x, y, rx, ry);    
    }
    
}
