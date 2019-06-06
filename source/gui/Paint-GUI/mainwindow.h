#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <device.h>
#include <memory>
#include <command.h>
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

    void on_cmdResize_clicked();

    void canvasMouseMoved(int x, int y);

    void canvasMouseClicked(int x, int y);

    void canvasMouseRightClicked(int x, int y);

    void on_pushButton_Line_clicked();

    void on_pushButton_3_clicked();

private:
    void command_status_handler(Command::status status);

    std::unique_ptr<Command> current_command;
    Ui::MainWindow *ui;
    Paint::Canvas<QImageDevice> canvas;
};

#endif // MAINWINDOW_H
