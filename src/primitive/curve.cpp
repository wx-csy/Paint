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
#include <numeric>
#include <paint/paint.h>
#include <paint/primitive.h>
#include <paint/util.h>
#include <cassert>
#include "algo.h"

using std::lround;          // from <cmath>
using std::abs; 
using std::swap;            // from <utility>
using util::limit_range;

static void DrawEllipse_Midpoint(Paint::ImageDevice& device, Paint::RGBColor color,
        float x, float y, float rx, float ry) {
    long long
        ix = limit_range(x, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iy = limit_range(y, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        irx = limit_range(rx, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE), 
        iry = limit_range(ry, Paint::MIN_COORDINATE, Paint::MAX_COORDINATE),
        irx2 = irx * irx,
        iry2 = iry * iry;

    auto quaddraw = [&] (int cx, int cy) {
        device.setPixel(ix + cx, iy + cy, color);
        device.setPixel(ix - cx, iy + cy, color);
        device.setPixel(ix + cx, iy - cy, color);
        device.setPixel(ix - cx, iy - cy, color);
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

template <typename T1, typename T2>
static void draw_curve_recursive(float tl, float tr, Paint::PointF pl, Paint::PointF pr, T1&& fn, T2&& setpixel) {
    if ((Paint::pf2pi(pl) - Paint::pf2pi(pr)).lmax() <= 1) return;
    float tmid = (tl + tr) / 2.0f;
    Paint::PointF pmid = fn(tmid);
    draw_curve_recursive(tl, tmid, pl, pmid, fn, setpixel);
    setpixel(lround(pmid.x), lround(pmid.y));
    draw_curve_recursive(tmid, tr, pmid, pr, fn, setpixel);
}

template <typename T1, typename T2>
static void draw_curve_recursive_wrapper(float tl, float tr, T1&& fn, T2&& setpixel) {
    Paint::PointF pl = fn(tl), pr = fn(tr);
    setpixel((int)pl.x, (int)pl.y);
    draw_curve_recursive(tl, tr, pl, pr, fn, setpixel);
    setpixel((int)pr.x, (int)pr.y);
}

namespace Paint {

    //
    // class Ellipse : public Element
    //
    void Ellipse::paint(ImageDevice& device) {
        DrawEllipse_Midpoint(device, color, x, y, rx, ry);    
    }

    void Ellipse::scale(float x, float y, float s) {
        std::tie(this->x, this->y) = rel_scale(x, y, this->x, this->y, s);
        rx *= s; ry *= s;
    }

    //
    // class ParametricCurve : public Element
    //

    void ParametricCurve::paint(Paint::ImageDevice &device) {
        draw_curve_recursive_wrapper(0.0f, 1.0f,
            [this] (float t) { return eval(t); },
            [&] (int x, int y) { device.setPixel(x, y, color); } );
    }

    //
    // class Bezier : public ParametricCurve
    //

    PointF Bezier::eval(float t) {
        std::vector<PointF> pts(points.begin(), points.end());
        while (pts.size() > 1) {
            for (size_t i = 0; i < pts.size() - 1; i++)
                pts[i] = t * pts[i] + (1.0 - t) * pts[i+1];
            pts.pop_back();
        }
        return pts[0];
    }

    void Bezier::translate(float dx, float dy) {
        for (auto& p : points) p += PointF(dx, dy);
    }

    void Bezier::rotate(float x, float y, float rdeg) {
        float mat[2][2];
        init_rotate_matrix(rdeg, mat);
        for (auto& p : points)
            std::tie(p.x, p.y) = rel_mat_apply(x, y, p.x, p.y, mat);
    }

    void Bezier::scale(float x, float y, float s) {
        for (auto& p : points)
            std::tie(p.x, p.y) = rel_scale(x, y, p.x, p.y, s);
    }

    //
    // class BSpline : public ParametricCurve
    //
    BSpline::BSpline(std::vector<PointF> points, RGBColor color, size_t order) :
            ParametricCurve(color), order(order), points(std::move(points)) {
        if (points.size() <= order)
            throw std::invalid_argument("number of points must be greater than order");
        size_t nknot = this->points.size() + order;
        knot.resize(nknot + 1);
        for (size_t i = 0; i <= nknot; i++) knot[i] = 1.0f * i / nknot;
        tl = float(order) / (order + this->points.size());
        tr = 1.0f - tl;
    }

    PointF BSpline::eval(float t) {
        t = tl + t * (tr - tl);
        std::vector<float> w(knot.size() - 1);
        for (size_t i = 0; i < knot.size() - 1; i++)
            w[i] = (t >= knot[i] and t < knot[i+1] ? 1.0f : 0.0f);
        if (t < knot.front()) w.front() = 1.0f;
        if (t >= knot.back()) w.back() = 1.0f;
        for (size_t r = 1; r <= order; r++) {
            for (size_t i = 0; i < w.size() - 1; i++) {
                w[i] =
                    w[i] * (t - knot[i]) / (knot[i+r] - knot[i]) +
                    w[i+1] * (knot[i+r+1] - t) / (knot[i+r+1] - knot[i+1]);
            }
            w.pop_back();
        }
        return inner_product(w.begin(), w.end(), points.begin(), PointF());
    }

    void BSpline::translate(float dx, float dy) {
        for (auto& p : points) p += PointF(dx, dy);
    }

    void BSpline::rotate(float x, float y, float rdeg) {
        float mat[2][2];
        init_rotate_matrix(rdeg, mat);
        for (auto& p : points)
            std::tie(p.x, p.y) = rel_mat_apply(x, y, p.x, p.y, mat);
    }

    void BSpline::scale(float x, float y, float s) {
        for (auto& p : points)
            std::tie(p.x, p.y) = rel_scale(x, y, p.x, p.y, s);
    }
 }
