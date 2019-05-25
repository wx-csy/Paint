#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAbout_Paint_triggered()
{
    QMessageBox::information(this, "About Paint", "Paint, a simple rasterization tool\nCopyright (C) 2019 Chen Shaoyuan\n");
}


void MainWindow::on_cmdResize_clicked()
{
    int width = QInputDialog::getInt(this, "Resize", "Please specify width of the canvas:", 800, 600, 1200);
    int height = QInputDialog::getInt(this, "Resize", "Please specify height of the canvas:", 800, 600, 1200);
    ui->graphicsView->resize(width, height);
    ui->scrollAreaWidgetContents->setMinimumSize(width, height);
}
