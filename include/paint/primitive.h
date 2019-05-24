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

#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include <paint/device.h>

namespace Paint {
    class Primitive {
    protected:
        RGBColor color;
        explicit Primitive(RGBColor color) : color(color) {}

    public:
        virtual void paint(ImageDevice& device) = 0;
        virtual void translate(float dx, float dy) = 0;
        virtual void rotate(float x, float y, float rdeg) = 0;
        virtual void scale(float x, float y, float s) = 0;
        virtual ~Primitive() = default;
    };


    enum class CurveDrawingAlgorithm : int { Bezier, BSpline };
    enum class LineClippingAlgorithm : int { CohenSutherland, LiangBarsky };

    class Line : public Primitive {
    public:
        enum class Algorithm : int { DDA, Bresenham };

    private:
        PointF p1, p2;
        Algorithm algo;

    public:
        Line(PointF p1, PointF p2, RGBColor color, Algorithm algo) :
            Primitive(color), p1(p1), p2(p2), algo(algo) {};

        void paint(ImageDevice& device) override;

        void translate(float dx, float dy) override {
            p1.x += dx; p1.y += dy;
            p2.x += dx; p2.y += dy;
        }

        void rotate(float x, float y, float rdeg) override;

        void scale(float x, float y, float s) override;

        void clip(float x1, float y1, float x2, float y2,
                  LineClippingAlgorithm algo);
    };

    class Polygon : public Primitive {
    private:
        std::vector<std::pair<float, float>> points;
        Line::Algorithm algo;

    public:
        Polygon(std::vector<std::pair<float, float>> points,
                RGBColor color, Line::Algorithm algo) :
            Primitive(color), points(std::move(points)), algo(algo) {}

        void paint(ImageDevice& device) override;

        void translate(float dx, float dy) override {
            for (auto& p : points) {
                p.first += dx;
                p.second += dy;
            }
        }

        void rotate(float x, float y, float rdeg) override;

        void scale(float x, float y, float s) override;
    };

    class Ellipse : public Primitive {
    private:
        float x, y, rx, ry;

    public:
        Ellipse(float x, float y, float rx, float ry, RGBColor color) :
            Primitive(color), x(x), y(y), rx(rx), ry(ry) {}

        void paint(ImageDevice& device) override;

        void translate(float dx, float dy) override {
            x += dx;
            y += dy;
        }

        void rotate(float x, float y, float rdeg) override {
            throw std::runtime_error("not supported");
        }

        void scale(float x, float y, float s) override;

    };

    class Curve : public Primitive {
    protected:
        std::vector<PointF> points;


        Curve(std::vector<PointF> points, RGBColor color) :
            Primitive(color), points(std::move(points)) { }

        void paint(ImageDevice& device) override = 0;

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

        void paint(ImageDevice& device) override;
    };
}

#endif //PAINT_PRIMITIVE_H
