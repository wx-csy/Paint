#ifndef __PAINT_H__
#define __PAINT_H__

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <utility>

namespace Paint {
    constexpr int MIN_COORDINATE = -8192, MAX_COORDINATE = 8192; 
    struct RGBColor { 
        std::uint8_t red, green, blue;

        constexpr RGBColor(std::uint8_t red = 0, 
                 std::uint8_t green = 0, 
                 std::uint8_t blue = 0) :
            red(red), green(green), blue(blue) { }
    };

    namespace Colors {
        constexpr RGBColor black(0, 0, 0);
        constexpr RGBColor white(255, 255, 255); 
    }
    
    class Canvas {
    protected:
        std::size_t width, height;
        Canvas(std::size_t width, std::size_t height) :
            width(width), height(height) { }

    public:
        virtual RGBColor getPixel(int x, int y) const = 0;
        virtual void setPixel(int x, int y, RGBColor color) = 0;
        virtual void reset(std::size_t width, std::size_t height) = 0;
        virtual void clear(RGBColor color = Colors::white) {
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
        MemoryCanvas(std::size_t width = 800, std::size_t height = 600) : 
            Canvas(width, height), data(width * height)
        { }
        
        MemoryCanvas(const MemoryCanvas& other) = delete;
        MemoryCanvas(MemoryCanvas&& other) = delete;
        MemoryCanvas& operator = (const MemoryCanvas& other) = delete;
        MemoryCanvas& operator = (MemoryCanvas&& other) = delete;
        
        RGBColor getPixel(int x, int y) const override {
            if (x < 0 || y < 0 || 
                std::size_t(x) >= width || std::size_t(y) >= height) 
                throw std::range_error("pixel out of canvas");
            return data[width * y + x];
        }
        
        void setPixel(int x, int y, RGBColor color) override {
            if (x < 0 || y < 0 || 
                std::size_t(x) >= width || std::size_t(y) >= height) 
                return;
            data[width * y + x] = color;
        }
        
        void clear(RGBColor color = Colors::white) override {
            std::fill(data.begin(), data.end(), color);
        }

        void reset(std::size_t width, std::size_t height) {
            this->width = width;
            this->height = height;
            data.assign(width * height, RGBColor());
        }
    };
        
    class Element {
    protected:
        RGBColor color;
        Element(RGBColor color) : color(color) {}

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
        float x1, y1, x2, y2;
        LineDrawingAlgorithm algo;

    public:
        Line(float x1, float y1, float x2, float y2, 
                RGBColor color, LineDrawingAlgorithm algo) :
            Element(color), x1(x1), y1(y1), x2(x2), y2(y2), algo(algo) {};
                
        void paint(Canvas& canvas) override;

        void translate(float dx, float dy) override {
            x1 += dx; y1 += dy;
            x2 += dx; y2 += dy;
        }

        void rotate(float x, float y, float rdeg) override {
            throw std::runtime_error("not implemented");
        }

        void scale(float x, float y, float s) override {
            throw std::runtime_error("not implemented");
        }

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

        void rotate(float x, float y, float rdeg) override {
            throw std::runtime_error("not implemented");   
        }
        
        void scale(float x, float y, float s) override {
            throw std::runtime_error("not implemented");
        }

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
            throw std::runtime_error("not implemented");   
        }
        
        void scale(float x, float y, float s) override {
            throw std::runtime_error("not implemented");
        }

    };
}

#endif
