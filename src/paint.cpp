#include <cmath>
#include <utility>
#include "paint.h"
using std::lrint;           // from <cmath>
using std::abs; 
using std::swap;            // from <utility>

static void DrawLine_DDA(const Paint::Pen& pen, 
        float x1, float y1, float x2, float y2) {
    int ix1 = lrint(x1), iy1 = lrint(y1), ix2 = lrint(x2), iy2 = lrint(y2);
    if (ix1 == ix2 && iy1 == iy2) {
        pen(ix1, iy1);
    } else if (abs(ix1 - ix2) > abs(iy1 - iy2)) {
        if (ix1 > ix2) { swap(ix1, ix2); swap(iy1, iy2); };
        float slope = (y2 - y1) / (x2 - x1);
        for (int x = ix1; x <= ix2; x++) 
            pen(x, lrint(iy1 + slope * (x - ix1)));
    } else {
        if (iy1 > iy2) { swap(ix1, ix2); swap(iy1, iy2); };
        float slope = (x2 - x1) / (y2 - y1);
        for (int y = iy1; y <= iy2; y++) 
            pen(lrint(ix1 + slope * (y - iy1)), y);
    }
}

namespace Paint {
    //
    // class LineElement : public Element
    //
    void LineElement::paint(const Pen& pen) {
        DrawLine_DDA(pen, x1, y2, x2, y2);
    }
}
