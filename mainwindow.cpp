#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    size = 40;
    draw();
    connect(ui->pathfinder, QPushButton::clicked, this, MainWindow::before_start);
    connect(ui->refresh, QPushButton::clicked, this, MainWindow::clearWindow);
    connect(ui->refresh, QPushButton::clicked, grid, Grid::reset_beg_end);
    connect(ui->pause, QPushButton::clicked, grid, Grid::killLoop);
    connect(ui->percent_open_nodes, QLineEdit::returnPressed, this, MainWindow::readVal);
    connect(this, MainWindow::sendVal, grid, Grid::openRandom);
    connect(grid, Grid::sendSteps, this, MainWindow::setLCDSteps);
    connect(grid, Grid::sendTime, this, MainWindow::setLCDTime);
    ui->lcdSteps->setPalette(Qt::black);
    ui->lcdTime->setPalette(Qt::black);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::draw()
{
    grid = new Grid(size);
    qreal xOffset = 0;

    for (int r = 0; r < size; r++)
    {
        r % 2 == 0 ? xOffset = 0 : xOffset = 0.5;

        for (int c = 0; c < size; c++)
        {
            grid->XY(r, c)->setPolygon(grid->XY(r, c)->getHex());
            scene->addItem(grid->XY(r, c));
            grid->XY(r, c)->setPos((c + xOffset) * 17, r * 15);

            connect(grid->XY(r, c), Hex::showCoordinates, ui->position, QLabel::setText);
            connect(grid->XY(r, c), Hex::statement, ui->statements, QLabel::setText);
            connect(grid->XY(r, c), Hex::start, ui->begin, QLabel::setText);
            connect(grid->XY(r, c), Hex::stop, ui->end, QLabel::setText);
        }
    }
}

void MainWindow::clearWindow()
{
    for (int r = 0; r < size; r++)
        for (int c = 0; c < size; c++)
            grid->XY(r, c)->reset();
    ui->percent_open_nodes->setText("0");
    ui->begin->setText("Początek: ");
    ui->end->setText("Koniec: ");
    ui->lcdTime->display(0);
    ui->lcdSteps->display(0.0);
}

void MainWindow::readVal()
{
    QString temp = ui->percent_open_nodes->text();
    bool ok;
    int num = temp.toInt(&ok);

    if (ok)
        emit sendVal(num);
}

void MainWindow::before_start()
{
    ui->statements->setText("Wybierz sposób wyszukiwania, zaznaczając jedną z dostępnych opcji po prawej stronie");
    delay(2000);
    ui->statements->setText("Wybierz algorytm szukania a następnie wstaw przeszkody i wagi lub kliknij \"Szukaj\"");
}


void MainWindow::on_force_0_clicked()
{
    grid->setForce(0);
    ui->frame->setStyleSheet("background-color:white;");
    ui->frame_2->setStyleSheet("background-color:white;");
}
void MainWindow::on_force_01_clicked()
{
    grid->setForce(1);
    ui->frame->setStyleSheet("background-color:rgb(200, 248, 255);");
    ui->frame_2->setStyleSheet("background-color:rgb(200, 248, 255);");
}
void MainWindow::on_force_02_clicked()
{
    grid->setForce(2);
    ui->frame->setStyleSheet("background-color:rgb(135, 206, 250);");
    ui->frame_2->setStyleSheet("background-color:rgb(135, 206, 250);");

}
void MainWindow::on_force_03_clicked()
{
    grid->setForce(3);
    ui->frame->setStyleSheet("background-color:rgb(30, 144, 255);");
    ui->frame_2->setStyleSheet("background-color:rgb(30, 144, 255);");

}
void MainWindow::on_force_04_clicked()
{
    grid->setForce(4);
    ui->frame->setStyleSheet("background-color:rgb(25, 25, 112);");
    ui->frame_2->setStyleSheet("background-color:rgb(25, 25, 112);");
}
void MainWindow::on_obstacle_clicked()
{
    grid->setForce(-1);
    ui->frame->setStyleSheet("background-color:red;");
    ui->frame_2->setStyleSheet("background-color:red;");
}

void MainWindow::setLCDSteps(int c)
{
    ui->lcdSteps->display(c);
}

void MainWindow::setLCDTime(double t)
{
    ui->lcdTime->display(t);
}

void MainWindow::on_percolates_clicked()
{
    disconnect(ui->pathfinder, QPushButton::clicked, this, MainWindow::before_start);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_random);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_force);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_force_unique);
    ui->statements->setText("Kliknij \"Szukaj\"");
    connect(ui->pathfinder, QPushButton::clicked, grid, Grid::percolation);
}

void MainWindow::on_random_clicked()
{
    disconnect(ui->pathfinder, QPushButton::clicked, this, MainWindow::before_start);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::percolation);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_force);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_force_unique);
    ui->statements->setText("Wstaw przeszkody i wagi lub kliknij \"Szukaj\"");
    connect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_random);
}

void MainWindow::on_random_force_clicked()
{
    disconnect(ui->pathfinder, QPushButton::clicked, this, MainWindow::before_start);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::percolation);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_random);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_force_unique);
    ui->force_01->setText("5%");
    ui->force_02->setText("15%");
    ui->force_03->setText("20%");
    ui->force_04->setText("60%");
    ui->statements->setText("Wstaw przeszkody i wagi lub kliknij \"Szukaj\"");
    connect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_force);
}

void MainWindow::on_random_force_unique_clicked()
{
    disconnect(ui->pathfinder, QPushButton::clicked, this, MainWindow::before_start);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::percolation);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_random);
    disconnect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_force);
    ui->force_01->setText("5%");
    ui->force_02->setText("15%");
    ui->force_03->setText("20%");
    ui->force_04->setText("60%");
    ui->statements->setText("Wstaw przeszkody i wagi lub kliknij \"Szukaj\"");
    connect(ui->pathfinder, QPushButton::clicked, grid, Grid::find_path_force_unique);
}
