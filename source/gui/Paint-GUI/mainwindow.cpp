#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "command.h"
#include <QImage>
#include <QMessageBox>
#include <QInputDialog>
#include <QLayout>
#include <QColorDialog>
#include <QTextStream>
#include <QDesktopServices>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->setMouseTracking(true);
    ui->label->resize(canvas.getWidth(), canvas.getHeight());
    ui->splitter->setStretchFactor(0, 2);
    ui->scrollAreaWidgetContents->setMinimumSize(canvas.getWidth(), canvas.getHeight());
    connect(ui->label, &MovableLabel::mouseMoved, this, &MainWindow::canvasMouseMoved);
    connect(ui->label, &MovableLabel::mouseClicked, this, &MainWindow::canvasMouseClicked);
    connect(ui->label, &MovableLabel::mouseRightClicked, this, &MainWindow::canvasMouseRightClicked);
    setColor(Paint::Colors::black);
    ui->primitiveList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->primitiveList->setModel(&model);
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

    // ui->primitiveList->setModel(&model);
}

void MainWindow::updateList() {
    QStringList list;
    for (auto& pr : canvas.primitives)
        list << (std::to_string(pr.first) + " " + pr.second->to_string()).c_str();
    model.setStringList(list);
}

void MainWindow::on_actionAbout_Paint_triggered()
{
    QMessageBox::information(this, "About Paint",
        "Paint, a simple rasterization tool\n"
        "Copyright (C) 2019 Chen Shaoyuan\n"
        "\n"
        "This program is free software: you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by "
        "the Free Software Foundation, either version 3 of the License, or "
        "(at your option) any later version.\n"
        "\n"
        "This program is distributed in the hope that it will be useful, "
        "but WITHOUT ANY WARRANTY; without even the implied warranty of "
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the "
        "GNU General Public License for more details.\n"
        "\n"
        "You should have received a copy of the GNU General Public License "
        "along with this program. If not, see <http://www.gnu.org/licenses/>.");
}


void MainWindow::on_actionOfficial_Website_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/wx-csy/Paint"));
}

void MainWindow::command_status_handler(Command::status status)
{
    switch (status) {
    case Command::ABORT :
        current_command = nullptr;
        render();
        updateList();
        ui->statusBar->showMessage("Aborted.");
        break;
    case Command::DONE :
        current_command = nullptr;
        render();
        updateList();
        ui->statusBar->showMessage("Done.");
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
    bool ok;
    int width = QInputDialog::getInt(this, "Resize", "Please specify width of the canvas:", 800, 100, 1200, 100, &ok);
    if (!ok) return;
    int height = QInputDialog::getInt(this, "Resize", "Please specify height of the canvas:", 600, 100, 1200, 100, &ok);
    if (!ok) return;
    ui->label->resize(width, height);
    ui->scrollAreaWidgetContents->setMinimumSize(width, height);
    canvas.reset(width, height);
    render();
}

void MainWindow::on_cmdLine_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    current_command.reset(new LineCommand(canvas, color, line_drawing_algo, ui->statusBar));
}

void MainWindow::on_cmdPolygon_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    current_command.reset(new PolygonCommand(canvas, color, line_drawing_algo, ui->statusBar));
}

void MainWindow::on_cmdEllipse_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    current_command.reset(new EllipseCommand(canvas, color, ui->statusBar));
}

void MainWindow::on_cmdBezier_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    current_command.reset(new BezierCommand(canvas, color, ui->statusBar));
}

void MainWindow::on_cmdBSpline_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    current_command.reset(new BSplineCommand(canvas, color, ui->statusBar));
}

void MainWindow::setColor(Paint::RGBColor color)
{
    QPalette palette;
    this->color = color;
    palette.setColor(QPalette::Background, QColor(color.red, color.green, color.blue));
    ui->colorLabel->setAutoFillBackground(true);
    ui->colorLabel->setPalette(palette);
}

void MainWindow::on_cmdChangeColor_clicked()
{
    auto newcolor = QColorDialog::getColor(Qt::white, this);
    if (newcolor.isValid())
        setColor(Paint::RGBColor(newcolor.red(), newcolor.green(), newcolor.blue()));
}

int MainWindow::getSeletectedPrimitiveIndex() {
    int rid = ui->primitiveList->currentIndex().row(), eid;
    if (rid < 0) return -1;
    // QTextStream takes a mutable pointer to QString.
    QString str(model.stringList().at(rid));
    QTextStream ss(&str);
    int id; ss >> id;
    if (canvas.primitives.count(id) == 0) return -1;
    return id;
}

void MainWindow::on_cmdMove_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    int eid = getSeletectedPrimitiveIndex();
    if (eid < 0) {
        QMessageBox::warning(this, "Paint", "Please select exactly one primitive!");
        return;
    }
    current_command.reset(new MoveCommand(canvas, eid, ui->statusBar));
}

void MainWindow::on_cmdRotate_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    int eid = getSeletectedPrimitiveIndex();
    if (eid < 0) {
        QMessageBox::warning(this, "Paint", "Please select exactly one primitive!");
        return;
    }
    current_command.reset(new RotateCommand(canvas, eid, ui->statusBar));
}

void MainWindow::on_cmdScale_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    int eid = getSeletectedPrimitiveIndex();
    if (eid < 0) {
        QMessageBox::warning(this, "Paint", "Please select exactly one primitive!");
        return;
    }
    current_command.reset(new ScaleCommand(canvas, eid, ui->statusBar));
}

void MainWindow::on_cmdClip_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    int eid = getSeletectedPrimitiveIndex();
    if (eid < 0) {
        QMessageBox::warning(this, "Paint", "Please select exactly one primitive!");
        return;
    }
    try {
        Paint::Line &line = dynamic_cast<Paint::Line&>(*canvas.primitives[eid]);
        current_command.reset(new ClipCommand(canvas, line, clip_algo, ui->statusBar));
    } catch (std::bad_cast&) {
        QMessageBox::warning(this, "Paint", "Clip operation is applicable to line only!");
        return;
    }
}

void MainWindow::on_cmdDelete_clicked()
{
    if (current_command) command_status_handler(current_command->abort());
    int eid = getSeletectedPrimitiveIndex();
    if (eid < 0) {
        QMessageBox::warning(this, "Paint", "Please select exactly one primitive!");
        return;
    }
    canvas.primitives.erase(eid);
    command_status_handler(Command::DONE);
}

void MainWindow::on_actionResize_triggered()
{
    on_cmdResize_clicked();
}

void MainWindow::on_actionLine_triggered()
{
    on_cmdLine_clicked();
}

void MainWindow::on_actionPolygon_triggered()
{
    on_cmdPolygon_clicked();
}

void MainWindow::on_actionEllipse_triggered()
{
    on_cmdEllipse_clicked();
}

void MainWindow::on_actionB_Spline_triggered()
{
    on_cmdBSpline_clicked();
}

void MainWindow::on_actionBezier_triggered()
{
    on_cmdBezier_clicked();
}

void MainWindow::on_actionMove_triggered()
{
    on_cmdMove_clicked();
}

void MainWindow::on_actionRotate_triggered()
{
    on_cmdRotate_clicked();
}

void MainWindow::on_actionScale_triggered()
{
    on_cmdScale_clicked();
}

void MainWindow::on_actionDelete_triggered()
{
    on_cmdDelete_clicked();
}

void MainWindow::on_actionChange_Color_triggered()
{
    on_cmdChangeColor_clicked();
}

void MainWindow::on_actionSave_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Save File");
    canvas.image.save(filename);
}

void MainWindow::on_actionLine_Algorithm_toggled(bool arg1)
{
    if (arg1) {
        line_drawing_algo = Paint::Line::Algorithm::DDA;
    } else {
        line_drawing_algo = Paint::Line::Algorithm::Bresenham;
    }
}

void MainWindow::on_actionClip_Algorithm_toggled(bool arg1)
{
    if (arg1) {
        clip_algo = Paint::LineClippingAlgorithm::CohenSutherland;
    } else {
        clip_algo = Paint::LineClippingAlgorithm::LiangBarsky;
    }
}
