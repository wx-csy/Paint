#include <cmath>
#include <utility>
#include <stdexcept>
#include "paint.h"
#include "util.h"
using std::lrint;           // from <cmath>
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
            canvas.setPixel(x, lrint(y1 + slope * (x - ix1)), color);
    } else {
        if (iy1 > iy2) { swap(ix1, ix2); swap(iy1, iy2); };
        float slope = (x2 - x1) / (y2 - y1);
        for (int y = iy1; y <= iy2; y++) 
            canvas.setPixel(lrint(x1 + slope * (y - iy1)), y, color);
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
            throw std::runtime_error("not implemented");
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
            DrawLine_DDA(canvas, color, 
                points.front().first, points.front().second,
                points.back().first, points.back().second);
            break;
        case LineDrawingAlgorithm::Bresenham :
            throw std::runtime_error("not implemented");
            break;
        default:
            throw std::invalid_argument("unknown algorithm");
        }
    }
}
