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
};

#endif // MAINWINDOW_H
