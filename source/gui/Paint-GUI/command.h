#ifndef COMMAND_H
#define COMMAND_H

#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "device.h"
#include "paint/canvas.h"
#include "paint/primitive.h"
#include <memory>
#include <QStatusBar>

class Command {
public:
    enum status : int {
        DONE = 0,
        CONTINUE = 1,
        REFRESH = 2,
        ABORT = 3,
    };

    Paint::Canvas<QImageDevice>& canvas;
    explicit Command(Paint::Canvas<QImageDevice>& canvas,
                     QStatusBar *statusBar = nullptr) :
        canvas(canvas), statusBar(statusBar) {}
    virtual ~Command();

    virtual status mouseClick(int x, int y) = 0;
    virtual status mouseRightClick(int x, int y) { return abort(); }
    virtual status mouseMove(int x, int y) = 0;
    virtual status abort() { return ABORT; }

private:
    QStatusBar *statusBar = nullptr;

protected:
    void showStatusTip(const QString& text) {
        if (statusBar)
            statusBar->showMessage(text);
    }
};

class LineCommand : public Command {
public:
    explicit LineCommand(Paint::Canvas<QImageDevice>& canvas, Paint::RGBColor color, Paint::Line::Algorithm algo,
                         QStatusBar *statusBar = nullptr);
    ~LineCommand() override = default;
    status mouseClick(int x, int y) override;
    status mouseMove(int x, int y) override;
    status abort() override;

private:
    int phase = 0;
    Paint::Line &line;
    int elem_id;
};

class PolygonCommand : public Command {
public:
    explicit PolygonCommand(Paint::Canvas<QImageDevice>& canvas, Paint::RGBColor color, Paint::Line::Algorithm algo,
                            QStatusBar *statusBar = nullptr);
    ~PolygonCommand() override = default;
    status mouseClick(int x, int y) override;
    status mouseRightClick(int x, int y) override;
    status mouseMove(int x, int y) override;
    status abort() override;

private:
    Paint::Polygon &polygon;
    int elem_id;
};

class EllipseCommand : public Command {
public:
    explicit EllipseCommand(Paint::Canvas<QImageDevice>& canvas, Paint::RGBColor color,
                            QStatusBar *statusBar = nullptr);
    ~EllipseCommand() override = default;
    status mouseClick(int x, int y) override;
    status mouseMove(int x, int y) override;
    status abort() override;

private:
    int phase = 0;
    Paint::Ellipse &ellipse;
    int elem_id;
};

class BezierCommand : public Command {
public:
    explicit BezierCommand(Paint::Canvas<QImageDevice>& canvas, Paint::RGBColor color,
                           QStatusBar *statusBar = nullptr);
    ~BezierCommand() override = default;
    status mouseClick(int x, int y) override;
    status mouseRightClick(int x, int y) override;
    status mouseMove(int x, int y) override;
    status abort() override;

private:
    Paint::Bezier &bezier;
    int elem_id;
};

class BSplineCommand : public Command {
public:
    explicit BSplineCommand(Paint::Canvas<QImageDevice>& canvas, Paint::RGBColor color,
                            QStatusBar *statusBar = nullptr);
    ~BSplineCommand() override = default;
    status mouseClick(int x, int y) override;
    status mouseRightClick(int x, int y) override;
    status mouseMove(int x, int y) override;
    status abort() override;

private:
    Paint::BSpline &bspline;
    int elem_id;
};

class MoveCommand : public Command {
public:
    explicit MoveCommand(Paint::Canvas<QImageDevice>& canvas, int id,
                         QStatusBar *statusBar = nullptr);
    ~MoveCommand() override = default;
    status mouseClick(int x, int y) override;
    status mouseMove(int x, int y) override;

private:
    Paint::Primitive &primitive;
    Paint::PointF lastv;
    int phase = 0;
};

class RotateCommand : public Command {
public:
    explicit RotateCommand(Paint::Canvas<QImageDevice>& canvas, int id,
                          QStatusBar *statusBar = nullptr);
    ~RotateCommand() override = default;
    status mouseClick(int x, int y) override;
    status mouseMove(int x, int y) override;

private:
    Paint::Primitive &primitive;
    Paint::PointF center, base;
    int phase = 0;
};


class ScaleCommand : public Command {
public:
    explicit ScaleCommand(Paint::Canvas<QImageDevice>& canvas, int id,
                          QStatusBar *statusBar = nullptr);
    ~ScaleCommand() override = default;
    status mouseClick(int x, int y) override;
    status mouseMove(int x, int y) override;

private:
    Paint::Primitive &primitive;
    Paint::PointF center, base;
    int phase = 0;
};


class ClipCommand : public Command {
public:
    explicit ClipCommand(Paint::Canvas<QImageDevice>& canvas,
                         Paint::Line &line,
                         Paint::LineClippingAlgorithm algo,
                         QStatusBar *statusBar = nullptr);
    ~ClipCommand() override;
    status mouseClick(int x, int y) override;
    status mouseMove(int x, int y) override;

private:
    void updatePolygon();
    Paint::Line &line;
    Paint::LineClippingAlgorithm algo;
    Paint::PointF cd1, cd2;
    int boxid = -1;
    Paint::Polygon *box;
    int phase = 0;
};

#endif // COMMAND_H
