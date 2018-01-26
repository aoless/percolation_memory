#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QInputDialog>
#include "grid.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    Ui::MainWindow * ui;
    QGraphicsScene * scene;
    Grid * grid;
    int size;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void draw();

signals:
    void sendVal(int val);

private slots:
    void on_force_0_clicked();
    void on_force_01_clicked();
    void on_force_02_clicked();
    void on_force_03_clicked();
    void on_force_04_clicked();
    void clearWindow();
    void readVal();
    void on_obstacle_clicked();
    void setLCDSteps(int c);
    void setLCDTime(double t);
    void on_random_clicked();
    void on_random_force_clicked();
    void on_random_force_unique_clicked();
    void on_percolates_clicked();
    void before_start();
};

#endif // MAINWINDOW_H
