#include "command.h"
#include "paint/canvas.h"
#include <assert.h>
#include <cmath>
#include <QMessageBox>
using namespace std;

// trivial destructor

Command::~Command() {}

// LineCommand

LineCommand::LineCommand(Paint::Canvas<QImageDevice>& canvas, Paint::RGBColor color, Paint::Line::Algorithm algo,
                         QStatusBar *statusBar) :
    Command(canvas, statusBar),
    line(*new Paint::Line(Paint::PointF(), Paint::PointF(), color, algo)),
    elem_id(canvas.add_primitive(&line))
{
    // showStatusTip("Please left click to specify the start point.");
}

Command::status LineCommand::mouseClick(int x, int y) {
    if (phase == 1) {
        return Command::DONE;
    } else {
        phase++;
        showStatusTip("Please left click to specify the end point.");
        return Command::CONTINUE;
    }
}

Command::status LineCommand::mouseMove(int x, int y) {
    if (phase == 0) {
        line.p1 = line.p2 = Paint::PointF(x, y);
    } else if (phase == 1) {
        line.p2 = Paint::PointF(x, y);
    }
    return Command::REFRESH;
}

Command::status LineCommand::abort() {
    canvas.primitives.erase(elem_id);
    return Command::ABORT;
}

// PolygonCommand

PolygonCommand::PolygonCommand(Paint::Canvas<QImageDevice>& canvas, Paint::RGBColor color, Paint::Line::Algorithm algo,
                               QStatusBar *statusBar) :
    Command(canvas, statusBar),
    polygon(*new Paint::Polygon({{0, 0}}, color, algo)),
    elem_id(canvas.add_primitive(&polygon))
{
    // showStatusTip("Please left click to sepcify the start vertex.");
}

Command::status PolygonCommand::mouseClick(int x, int y) {
    polygon.points.emplace_back(x, y);
    showStatusTip("Please left click to add a vertex, or right click to finish.");
    return Command::REFRESH;
}

Command::status PolygonCommand::mouseMove(int x, int y) {
    polygon.points.back() = {x, y};
    return Command::REFRESH;
}

Command::status PolygonCommand::mouseRightClick(int x, int y) {
    return Command::DONE;
}

Command::status PolygonCommand::abort() {
    canvas.primitives.erase(elem_id);
    return Command::ABORT;
}

// EllipseCommand

EllipseCommand::EllipseCommand(Paint::Canvas<QImageDevice>& canvas, Paint::RGBColor color,
                               QStatusBar *statusBar) :
    Command(canvas, statusBar),
    ellipse(*new Paint::Ellipse(0, 0, 0, 0, color)),
    elem_id(canvas.add_primitive(&ellipse))
{
    // showStatusTip("Please left click to sepcify the center.");
}

Command::status EllipseCommand::mouseClick(int x, int y) {
    if (phase == 1) {
        return Command::DONE;
    } else {
        phase++;
        showStatusTip("Please left click to sepcify the size.");
        return Command::CONTINUE;
    }
}

Command::status EllipseCommand::mouseMove(int x, int y) {
    if (phase == 0) {
        ellipse.x = x;
        ellipse.y = y;
    } else if (phase == 1) {
        ellipse.rx = fabs(x - ellipse.x);
        ellipse.ry = fabs(y - ellipse.y);
    }
    return Command::REFRESH;
}

Command::status EllipseCommand::abort() {
    canvas.primitives.erase(elem_id);
    return Command::ABORT;
}

// BezierCommand

BezierCommand::BezierCommand(Paint::Canvas<QImageDevice>& canvas, Paint::RGBColor color,
                             QStatusBar *statusBar) :
    Command(canvas, statusBar),
    bezier(*new Paint::Bezier({Paint::PointF(0, 0)}, color)),
    elem_id(canvas.add_primitive(&bezier))
{
    // showStatusTip("Please left click to sepcify the start control point.");
}

Command::status BezierCommand::mouseClick(int x, int y) {
    bezier.points.emplace_back(x, y);

    showStatusTip("Please left click to add a control point, or right click to finish.");
    return Command::REFRESH;
}

Command::status BezierCommand::mouseMove(int x, int y) {
    bezier.points.back() = Paint::PointF(x, y);
    return Command::REFRESH;
}

Command::status BezierCommand::mouseRightClick(int x, int y) {
    return Command::DONE;
}

Command::status BezierCommand::abort() {
    canvas.primitives.erase(elem_id);
    return Command::ABORT;
}

// BSplineCommand

BSplineCommand::BSplineCommand(Paint::Canvas<QImageDevice>& canvas, Paint::RGBColor color,
                               QStatusBar *statusBar) :
    Command(canvas, statusBar),
    bspline(*new Paint::BSpline({Paint::PointF(0, 0)}, color)),
    elem_id(canvas.add_primitive(&bspline))
{   
    // showStatusTip("Please left click to sepcify the start control point.");
}

Command::status BSplineCommand::mouseClick(int x, int y) {
    bspline.points.emplace_back(x, y);
    bspline.update_knot();
    showStatusTip("Please left click to add a control point, or right click to finish.");
    return Command::REFRESH;
}

Command::status BSplineCommand::mouseMove(int x, int y) {
    bspline.points.back() = Paint::PointF(x, y);
    return Command::REFRESH;
}

Command::status BSplineCommand::mouseRightClick(int x, int y) {
    return Command::DONE;
}

Command::status BSplineCommand::abort() {
    canvas.primitives.erase(elem_id);
    return Command::ABORT;
}

// MoveCommand

MoveCommand::MoveCommand(Paint::Canvas<QImageDevice>& canvas, int id,
                         QStatusBar *statusBar) :
    Command(canvas, statusBar), primitive(*canvas.primitives[id])
{

}

Command::status MoveCommand::mouseClick(int x, int y) {
    if (phase == 1) {
        return Command::DONE;
    } else {
        phase++;
        lastv = Paint::PointF(x, y);
        showStatusTip("Please left click to finish.");
        return Command::CONTINUE;
    }
}

Command::status MoveCommand::mouseMove(int x, int y) {
    if (phase == 1){
        Paint::PointF newv(x, y);
        primitive.translate(newv.x - lastv.x, newv.y - lastv.y);
        lastv = newv;
        return Command::REFRESH;
    } else {
        return Command::CONTINUE;
    }
}

// RotateCommand

RotateCommand::RotateCommand(Paint::Canvas<QImageDevice>& canvas, int id,
                             QStatusBar *statusBar) :
    Command(canvas, statusBar), primitive(*canvas.primitives[id])
{

}

Command::status RotateCommand::mouseClick(int x, int y) {
    if (phase == 0) {
        center = Paint::PointF(x, y);
        phase++;
        showStatusTip("Please left click to specify base point.");
        return Command::CONTINUE;
    } else if (phase == 1) {
        base = Paint::PointF(x, y);
        phase++;
        showStatusTip("Please left click to specify finish.");
        return Command::CONTINUE;
    } else if (phase == 2) {
        return Command::DONE;
    }
    assert(0);
}

Command::status RotateCommand::mouseMove(int x, int y) try {
    if (phase == 2) {
        Paint::PointF nbase(x, y);
        float ratio = (nbase - center).arg() - (base - center).arg();
        primitive.rotate(center.x, center.y, ratio / acos(-1) * 180.0);
        base = nbase;
        return status::REFRESH;
    } else {
        return status::CONTINUE;
    }
} catch (std::runtime_error& error) {
    QMessageBox::warning(nullptr, "Paint", QString("Runtime error: ") + error.what());
    return status::ABORT;
}

// ScaleCommand

ScaleCommand::ScaleCommand(Paint::Canvas<QImageDevice>& canvas, int id,
                           QStatusBar *statusBar) :
    Command(canvas, statusBar), primitive(*canvas.primitives[id])
{

}

Command::status ScaleCommand::mouseClick(int x, int y) {
    if (phase == 0) {
        center = Paint::PointF(x, y);
        phase++;
        showStatusTip("Please left click to specify base point.");
        return Command::CONTINUE;
    } else if (phase == 1) {
        base = Paint::PointF(x, y);
        phase++;
        showStatusTip("Please left click to specify finish.");
        return Command::CONTINUE;
    } else if (phase == 2) {
        return Command::DONE;
    }
    assert(0);
}

Command::status ScaleCommand::mouseMove(int x, int y) {
    if (phase == 2) {
        Paint::PointF nbase(x, y);
        float ratio = (nbase - center).abs() / (base - center).abs();
        primitive.scale(center.x, center.y, ratio);
        base = nbase;
        return status::REFRESH;
    } else {
        return status::CONTINUE;
    }
}

// ClipCommand

ClipCommand::ClipCommand(Paint::Canvas<QImageDevice>& canvas,
                         Paint::Line &line,
                         Paint::LineClippingAlgorithm algo,
                         QStatusBar *statusBar) :
    Command(canvas, statusBar), line(line), algo(algo)
{

}

void ClipCommand::updatePolygon() {
    box->points[0] = {cd1.x, cd1.y};
    box->points[1] = {cd1.x, cd2.y};
    box->points[2] = {cd2.x, cd2.y};
    box->points[3] = {cd2.x, cd1.y};
}

Command::status ClipCommand::mouseMove(int x, int y) {
    if (phase == 1) {
        cd2 = Paint::PointF(x, y);
        updatePolygon();
        return Command::REFRESH;
    } else {
        return Command::CONTINUE;
    }
}

Command::status ClipCommand::mouseClick(int x, int y) {
    if (phase == 0) {
        cd1 = cd2 = Paint::PointF(x, y);
        box = new Paint::Polygon(std::vector<pair<float, float>>(4, {0.0, 0.0}),
                    Paint::RGBColor(127, 127, 127), Paint::Line::Algorithm::DDA);
        boxid = canvas.add_primitive(box);
        updatePolygon();
        phase++;
        showStatusTip("Please left click to finish.");
        return Command::CONTINUE;
    } else {
        try {
            line.clip(cd1.x, cd1.y, cd2.x, cd2.y, algo);
            return Command::DONE;
        } catch (std::runtime_error& error) {
            QMessageBox::warning(nullptr, "Paint", QString("Runtime error: ") + error.what());
            return Command::ABORT;
        }
    }
}

ClipCommand::~ClipCommand() {
    if (boxid >= 0) {
        canvas.primitives.erase(boxid);
    }
}
