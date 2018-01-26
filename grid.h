#ifndef GRID_H
#define GRID_H

#include "hex.h"
#include <QDebug>
#include <QVector>
#include <QMouseEvent>
#include <QApplication>
#include <algorithm>
#include <random>
#include <QtGlobal>
#include <QElapsedTimer>

using matrix = QVector<QVector<Hex*>>;

//obiekt zawierający w sobie obiekt Hex | w oknie wyświetlany jako sieć sześcianów
class Grid : public QObject
{
    Q_OBJECT
private:
    int size;
    matrix grid;
    Hex * begin;
    Hex * aim;
    QVector<int> neighbour;
    bool killLoopFlag;
    bool beginIsSet = false;
    int openPrev = 0;
public:
    Grid(int _size);
    ~Grid();
    Hex * XY(int x, int y);
    void mousePressEvent(QEvent *event);
    int force_random_unique(QVector<int> & tbl, int frc);
    void setForce(int f);
    int force_random(const QVector<int> &tbl, int frc);
public slots:
    void setSelectable();
    void selectEnd();
    void setBegin(int _x, int _y);
    void setAim(int _x, int _y);
    void find_path_random();
    void find_path_force();
    void find_path_force_unique();
    void reset_beg_end();
    void killLoop() { killLoopFlag = true; }
    void openRandom(int val);
    void percolation();
signals:
    void sendSteps(int c);
    void sendTime(double t);
};

#endif // GRID_H
