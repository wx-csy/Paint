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
    void mouseClicked(int x, int y);
    void mouseRightClicked(int x, int y);

private:
    void mouseMoveEvent(QMouseEvent *event) override {
        emit mouseMoved(event->x(), event->y());
    }

    void mousePressEvent(QMouseEvent *event) override {
        switch (event->button()) {
        case Qt::MouseButton::LeftButton :
            emit mouseClicked(event->x(), event->y());
            break;
        case Qt::MouseButton::RightButton  :
            emit mouseRightClicked(event->x(), event->y());
            break;
        default :
            break;
        }
    }

};

#endif // MOVABLELABEL_H
