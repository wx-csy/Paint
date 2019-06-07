#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <device.h>
#include <memory>
#include <command.h>
#include <QStringListModel>
#include "paint/canvas.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void render();

private slots:
    void on_actionAbout_Paint_triggered();

    void canvasMouseMoved(int x, int y);

    void canvasMouseClicked(int x, int y);

    void canvasMouseRightClicked(int x, int y);

    void on_cmdResize_clicked();

    void on_cmdLine_clicked();

    void on_cmdPolygon_clicked();

    void on_cmdEllipse_clicked();

    void on_cmdBezier_clicked();

    void on_cmdBSpline_clicked();

    void on_cmdChangeColor_clicked();

    void on_actionOfficial_Website_triggered();

    void on_cmdMove_clicked();

    void on_cmdScale_clicked();

    void on_cmdRotate_clicked();

    void on_cmdClip_clicked();

    void on_cmdDelete_clicked();

    void on_actionLine_triggered();

    void on_actionResize_triggered();

    void on_actionPolygon_triggered();

    void on_actionEllipse_triggered();

    void on_actionB_Spline_triggered();

    void on_actionBezier_triggered();

    void on_actionMove_triggered();

    void on_actionRotate_triggered();

    void on_actionScale_triggered();

    void on_actionDelete_triggered();

    void on_actionChange_Color_triggered();

    void on_actionSave_triggered();

    void on_actionLine_Algorithm_toggled(bool arg1);

    void on_actionClip_Algorithm_toggled(bool arg1);

private:
    void command_status_handler(Command::status status);
    void setColor(Paint::RGBColor color);
    void updateList();
    int getSeletectedPrimitiveIndex();

    std::unique_ptr<Command> current_command;
    Ui::MainWindow *ui;
    Paint::Canvas<QImageDevice> canvas;
    Paint::RGBColor color;
    QStringListModel model;

    Paint::Line::Algorithm line_drawing_algo = Paint::Line::Algorithm::Bresenham;
    Paint::LineClippingAlgorithm clip_algo = Paint::LineClippingAlgorithm::LiangBarsky;
};

#endif // MAINWINDOW_H
