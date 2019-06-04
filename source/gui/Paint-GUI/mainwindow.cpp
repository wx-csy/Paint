#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QMessageBox>
#include <QInputDialog>
#include <QLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->setMouseTracking(true);
    connect(ui->label, &MovableLabel::mouseMoved, this, &MainWindow::canvasMouseMoved);
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
    int width = QInputDialog::getInt(this, "Resize", "Please specify width of the canvas:", 600, 300, 1200);
    int height = QInputDialog::getInt(this, "Resize", "Please specify height of the canvas:", 800, 300, 1200);
    ui->label->resize(width, height);
    ui->scrollAreaWidgetContents->setMinimumSize(width, height);
    device.reset(width, height);
    ui->label->setPixmap(device.getPixmap());
}

void MainWindow::canvasMouseMoved(int x, int y)
{
    ui->label->setText(tr("(%1, %2)").arg(x).arg(y));
}
