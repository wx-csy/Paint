#include <cmath>
#include <utility>
#include <stdexcept>
#include "paint.h"
#include "util.h"
using std::lround;          // from <cmath>
using std::abs; 
using std::swap;            // from <utility>
using util::limit_range;

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
        float slope = (y2 - y1) / (x2 - x1);
        for (int x = ix1; x <= ix2; x++) 
            canvas.setPixel(x, lround(iy1 + slope * (x - ix1)), color);
    } else {
        if (iy1 > iy2) { swap(ix1, ix2); swap(iy1, iy2); };
        float slope = (x2 - x1) / (y2 - y1);
        for (int y = iy1; y <= iy2; y++) 
            canvas.setPixel(lround(ix1 + slope * (y - iy1)), y, color);
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
        long long f = 0;
        canvas.setPixel(ix1, negate ? -iy1 : iy1, color);
        for (int x = ix1, y = iy1; x < ix2; x++) {
            f += dy + dy - dx;
            if (f >= 0) { y++; f -= dx; } else { f += dx; }
            canvas.setPixel(x, negate ? -y : y, color);
        }
    } else {
        bool negate = false;
        if (iy1 > iy2) { swap(ix1, ix2); swap(iy1, iy2); }
        if (ix1 > ix2) { ix1 = -ix1; ix2 = -ix2; negate = true; }
        int dy = iy2 - iy1, dx = ix2 - ix1;
        long long f = 0;
        canvas.setPixel(negate ? -ix1 : ix1, iy1, color);
        for (int y = iy1, x = ix1; y < iy2; y++) { 
            f += dx + dx - dy;
            if (f >= 0) { x++; f -= dy; } else { f += dy; }
            canvas.setPixel(negate ? -x : x, y, color);
        }
    }
}

namespace Paint {
    //
    // class LineElement : public Element
    //
    void LineElement::paint(Canvas& canvas) {
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

    void LineElement::clip(float x1, float y1, float x2, float y2, 
            LineClippingAlgorithm algo) {
        throw std::runtime_error("not implemented");
    }

    //
    // class PolygonElement : public Element
    //
    void PolygonElement::paint(Canvas& canvas) {
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
}
