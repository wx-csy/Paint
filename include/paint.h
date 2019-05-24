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
    
    class Canvas {
    protected:
        size_t width, height;
        Canvas(size_t width, size_t height) :
            width(width), height(height) { }

    public:
        virtual RGBColor getPixel(ssize_t x, ssize_t y) const = 0;
        RGBColor getPixel(PointI pt) const {
            return getPixel(pt.x, pt.y);
        }
        virtual void setPixel(ssize_t x, ssize_t y, RGBColor color) = 0;
        void setPixel(PointI pt, RGBColor color) {
            setPixel(pt.x, pt.y, color);
        }
        virtual void reset(size_t width, size_t height) = 0;
        virtual void clear(RGBColor color) {
            for (size_t x = 0; x < width; x++)
                for (size_t y = 0; y < height; y++) 
                    setPixel(x, y, color);
        }
        virtual ~Canvas() = default;
    };

    class MemoryCanvas : public Canvas {
    private:
        std::vector<RGBColor> data;

    public:
        explicit MemoryCanvas(size_t width = 800, size_t height = 600) :
            Canvas(width, height), data(width * height)
        { }
        
        MemoryCanvas(const MemoryCanvas& other) = delete;
        MemoryCanvas(MemoryCanvas&& other) = delete;
        MemoryCanvas& operator = (const MemoryCanvas& other) = delete;
        MemoryCanvas& operator = (MemoryCanvas&& other) = delete;
        
        RGBColor getPixel(ssize_t x, ssize_t y) const override {
            if (x < 0 || y < 0 || 
                size_t(x) >= width || size_t(y) >= height) 
                throw std::range_error("pixel out of canvas");
            return data[width * y + x];
        }
        
        void setPixel(ssize_t x, ssize_t y, RGBColor color) override {
            if (x < 0 || y < 0 || 
                size_t(x) >= width || size_t(y) >= height) 
                return;
            data[width * y + x] = color;
        }
        
        void clear(RGBColor color) override {
            std::fill(data.begin(), data.end(), color);
        }

        void reset(size_t width, size_t height) override {
            this->width = width;
            this->height = height;
            data.assign(width * height, RGBColor());
        }
    };
        
    class Element {
    protected:
        RGBColor color;
        explicit Element(RGBColor color) : color(color) {}

    public:
        virtual void paint(Canvas& canvas) = 0;
        virtual void translate(float dx, float dy) = 0;
        virtual void rotate(float x, float y, float rdeg) = 0;
        virtual void scale(float x, float y, float s) = 0;
        virtual ~Element() = default;
    };

    enum class LineDrawingAlgorithm : int { DDA, Bresenham };
    enum class CurveDrawingAlgorithm : int { Bezier, BSpline };
    enum class LineClippingAlgorithm : int { CohenSutherland, LiangBarsky };

    class Line : public Element {
    private:
        PointF p1, p2;
        LineDrawingAlgorithm algo;

    public:
        Line(PointF p1, PointF p2, RGBColor color, LineDrawingAlgorithm algo) :
            Element(color), p1(p1), p2(p2), algo(algo) {};
                
        void paint(Canvas& canvas) override;

        void translate(float dx, float dy) override {
            p1.x += dx; p1.y += dy;
            p2.x += dx; p2.y += dy;
        }

        void rotate(float x, float y, float rdeg) override;

        void scale(float x, float y, float s) override;

        void clip(float x1, float y1, float x2, float y2,
                LineClippingAlgorithm algo);
    };

    class Polygon : public Element {
    private:
        std::vector<std::pair<float, float>> points;
        LineDrawingAlgorithm algo;
         
    public:
        Polygon(std::vector<std::pair<float, float>> points,
                RGBColor color, LineDrawingAlgorithm algo) :
            Element(color), points(std::move(points)), algo(algo) {}
             
        void paint(Canvas& canvas) override;

        void translate(float dx, float dy) override {
            for (auto& p : points) {
                p.first += dx;
                p.second += dy;
            }
        }

        void rotate(float x, float y, float rdeg) override;
        
        void scale(float x, float y, float s) override;
    };
    
    class Ellipse : public Element {
    private:
        float x, y, rx, ry; 
         
    public:
        Ellipse(float x, float y, float rx, float ry, RGBColor color) :
            Element(color), x(x), y(y), rx(rx), ry(ry) {}
             
        void paint(Canvas& canvas) override;

        void translate(float dx, float dy) override {
            x += dx;
            y += dy;
        }

        void rotate(float x, float y, float rdeg) override {
            throw std::runtime_error("not supported");
        }
        
        void scale(float x, float y, float s) override;

    };

    class Curve : public Element {
    protected:
        std::vector<PointF> points;


        Curve(std::vector<PointF> points, RGBColor color) :
            Element(color), points(std::move(points)) { }

        void paint(Canvas& canvas) override = 0;

        void translate(float dx, float dy) override {
            for (auto& p : points) {
                p.x += dx;
                p.y += dy;
            }
        }

        void rotate(float x, float y, float rdeg) override;
        
        void scale(float x, float y, float s) override;

    };

    class BezierCurve : public Curve {
    private:
        PointF eval(float t);

    public:
        BezierCurve(std::vector<PointF> points, RGBColor color) :
            Curve(std::move(points), color) { }
        void paint(Canvas& canvas) override;
    };
}

#endif
