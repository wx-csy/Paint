#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <device.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAbout_Paint_triggered();

    void on_cmdResize_clicked();

private:

    Ui::MainWindow *ui;
    QImageDevice device;
};

#endif // MAINWINDOW_H
