#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "command.h"
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
    ui->label->resize(canvas.getWidth(), canvas.getHeight());
    connect(ui->label, &MovableLabel::mouseMoved, this, &MainWindow::canvasMouseMoved);
    connect(ui->label, &MovableLabel::mouseClicked, this, &MainWindow::canvasMouseClicked);
    connect(ui->label, &MovableLabel::mouseRightClicked, this, &MainWindow::canvasMouseRightClicked);
    render();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::render()
{
    canvas.clear(Paint::Colors::white);
    canvas.paint();
    ui->label->setPixmap(canvas.getPixmap());
}

void MainWindow::on_actionAbout_Paint_triggered()
{
    QMessageBox::information(this, "About Paint", "Paint, a simple rasterization tool\nCopyright (C) 2019 Chen Shaoyuan\n");
}


void MainWindow::command_status_handler(Command::status status)
{
    switch (status) {
    case Command::ABORT : // fall through
    case Command::DONE :
        current_command = nullptr;
        render();
        break;
    case Command::CONTINUE :
        break;
    case Command::REFRESH :
        render();
        break;
    }
}

void MainWindow::canvasMouseMoved(int x, int y)
{
    // ui->label->setText(tr("(%1, %2)").arg(x).arg(y));
    if (current_command)
        command_status_handler(current_command->mouseMove(x, y));
}

void MainWindow::canvasMouseClicked(int x, int y)
{
    // this->setWindowTitle(tr("clicked (%1, %2)").arg(x).arg(y));
    if (current_command)
        command_status_handler(current_command->mouseClick(x, y));
}

void MainWindow::canvasMouseRightClicked(int x, int y)
{
    // this->setWindowTitle(tr("right clicked (%1, %2)").arg(x).arg(y));
    if (current_command)
        command_status_handler(current_command->mouseRightClick(x, y));
}

void MainWindow::on_cmdResize_clicked()
{
    int width = QInputDialog::getInt(this, "Resize", "Please specify width of the canvas:", 600, 300, 1200);
    int height = QInputDialog::getInt(this, "Resize", "Please specify height of the canvas:", 800, 300, 1200);
    ui->label->resize(width, height);
    ui->scrollAreaWidgetContents->setMinimumSize(width, height);
    canvas.reset(width, height);
    render();
}

void MainWindow::on_cmdLine_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    current_command.reset(new LineCommand(canvas, Paint::Colors::black, Paint::Line::Algorithm::Bresenham));
}

void MainWindow::on_cmdPolygon_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    current_command.reset(new PolygonCommand(canvas, Paint::Colors::black, Paint::Line::Algorithm::Bresenham));
}

void MainWindow::on_cmdEllipse_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    current_command.reset(new EllipseCommand(canvas, Paint::Colors::black));
}

void MainWindow::on_cmdBezier_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    current_command.reset(new BezierCommand(canvas, Paint::Colors::black));
}
