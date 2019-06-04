#ifndef MOVABLELABEL_H
#define MOVABLELABEL_H

#include <QLabel>
#include <QMouseEvent>

class MovableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit MovableLabel(QWidget* parent = nullptr) : QLabel(parent) {}
    ~MovableLabel() override = default;

signals:
    void mouseMoved(int x, int y);

private:
    void mouseMoveEvent(QMouseEvent *event) override {
        emit mouseMoved(event->x(), event->y());
    }
};

#endif // MOVABLELABEL_H
