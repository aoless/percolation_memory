#include "hex.h"
#include <QDebug>
#include <QtMath>

const int SIZE { 10 };

Hex::Hex(int x_, int y_)
{
    x = x_;
    y = y_;
    force = 0.0;
    mode = empty;
    this->setBrush(Qt::white);
    qreal edge = SIZE;
    qreal dx = qSqrt(3)/2 * edge;

    hexagon << QPointF(dx, -edge/2)
            << QPointF(0, -edge)
            << QPointF(-dx, -edge/2)
            << QPointF(-dx, edge/2)
            << QPointF(0, edge)
            << QPointF(dx, edge/2);

    setAcceptHoverEvents(true);
}

Hex::~Hex() {}

QVector<int> Hex::findNeighbour(int d)  //odnajduje i wzraca współrzędne sąsiada numer "d"
{
    QVector<int> vX(6);
    QVector<int> vY(6);

    if (x % 2 == 0)                     //rząd parzysty
    {
        vY = { 1, 0, -1, -1, -1, 0 };
        vX = { 0, -1, -1, 0, 1, 1 };
    }
    else                                //rząd nieparzysty
    {
        vY = { 1, 1, 0, -1, 0, 1 };
        vX = { 0, -1, -1, 0, 1, 1 };
    }

    QVector<int> neighbour = { vX[d] + x , vY[d] + y};
    return neighbour;
}

bool Hex::endNeighbour(int nX, int nY)  //sprawdza, czy wartości nX i nY nie są zgodne z którymś z sąsiadów Hexa "end"
{
    QVector<int> neighbour(2);

    for (int i = 0; i < 6; i++)
    {
        neighbour = findNeighbour(i);
        if (nX == neighbour[0] && nY == neighbour[1])
            return false;
    }
    return true;
}

void Hex::percolate_open()              //ustawia sześcian w sieci na otwarty, modyfikacja dla algorytmu "perkolacja"
{
    if (mode != obstacle)
    {
        mode = open;
        force = forceClicked;
        visited = true;
        setColor();
    }
}

void Hex::setToOpen()                   //ustawia sześcian w sieci na otwarty, modyfikacja dla pozostałym algorytmów
{
    if (mode != begin && mode != end && mode != obstacle)
    {
        mode = open;
        force = forceClicked;
        if (force == -1)
            force = 1;
        visited = false;
        setColor();
    }
}

void Hex::setForce()                    //ustawia wagi na podstawie wartości parametru "forceClicked"
{
    if (mode != begin && mode != end && mode != obstacle)
    {
        if (forceClicked == -1)
        {
            mode = obstacle;
            force = forceClicked;
            visited = false;
        }
        else
        {
            mode = open;
            force = forceClicked;
            visited = false;
        }
        setColor();
    }

}

void Hex::setToObstacle()               //ustawia sześcian jako zamkniety, lub otwiera w przeciwnym wypadku
{
    if (mode == empty)
    {
        mode = obstacle;
        setBrush(Qt::red);
        force = -1;
    }
    else if (mode == obstacle)
    {
        mode = empty;
        setBrush(Qt::white);
        force = 0;
    }
    else if (mode == open)
    {
        mode = obstacle;
        setBrush(Qt::red);
        force = -1;
    }
}

void Hex::setNeighbour()                //ustawia węzeł jako sąsiada
{
    if (mode == begin)
    {
        increaseForce();
        setBrush(Qt::green);
    }
    else if (mode != end && mode != obstacle)
    {
        mode = open;
        increaseForce();
    }
}

void Hex::increaseForce()               //inkrementuje wagę sześcianu
{
    if (force < 4)
    {
        force++;
        if (force == 4)
            visited = true;
    }
    else
        strongest++;

    setColor();
}

void Hex::setColor()                    //zmienia kolor sześcianu
{

    switch(force)
    {
    case 0:
        setBrush(Qt::white);
        strongest = 0;
        break;
    case 1:
        setBrush(QColor(200, 248, 255));
        strongest = 0;
        break;
    case 2:
        setBrush(QColor(135, 206, 250));
        strongest = 0;
        break;
    case 3:
        setBrush(QColor(30, 144, 255));
        strongest = 0;
        break;
    case 4:
        setBrush(QColor(25, 25, 112));
        if (strongest > 100)
            setBrush(QColor(25, 25, 50));
        break;
    case -1:
        setBrush(Qt::red);
        break;
    }
}

void Hex::changeForceClick()              //umożliwia wpływanie na wagę kliknięciem myszy
{
    if (mode == empty || mode == open || mode == obstacle)
    {
        force = forceClicked;
        if (force == -1)
            mode = obstacle;
        else
            mode = open;
        setColor();
    }
}

void Hex::reset()                          //resetuje sześcian w sieci
{
    mode = empty;
    force = 0;
    strongest = 0;
    setColor();
    beginIsPressed = false;
    selectable = false;
    visited = false;
    QString c = "Współrzędne: " + QString::number(0) + " , " + QString::number(0);
    emit statement(QString("Wybierz punkt poczatkowy..."));
    emit showCoordinates(c);
}

void Hex::refresh()                         //odświerza sześciany w sieci
{
    if (mode != begin && mode != end)
    {
        mode = empty;
        force = 0;
        setBrush(Qt::white);
        visited = false;
    }

}

void Hex::setBegin()                        //ustawia punkt początkowy
{
    mode = begin;
    setBrush(Qt::green);
    force = 0;
    emit beginIsSet(x, y);
    emit start(QString("Początek: " + QString::number(x) + " , " + QString::number(y)));
    emit statement(QString("Teraz wybierz punkt końcowy..."));
}

void Hex::setEnd()                          //ustawia punkt końcowy
{
    mode = end;
    setBrush(Qt::yellow);
    force = 0;
    emit endIsSet(x, y);
    emit stop(QString("Koniec: " + QString::number(x) + " , " + QString::number(y)));
}

void Hex::mousePressEvent(QGraphicsSceneMouseEvent *event) //definiuje akcje po użyciu przycisku myszy
{
    if (selectable)                         //jesli mozliwa jest zmiana wagi hexa
    {
        if (event->button() == Qt::LeftButton)
            changeForceClick();
        else if (event->button() == Qt::RightButton)
            setToObstacle();
    }
    else if (beginIsPressed)                //jesli nie mozna zmienic wagi, ale wybrano juz start, oczekuj na stop
    {
        if (event->button() == Qt::LeftButton)
        {
            setEnd();
            emit statement(QString("Wybierz algorytm szukania a następnie wstaw przeszkody i wagi lub kliknij \"Szukaj\""));
        }

        else
            emit statement(QString("Wybierz punkt koncowy..."));
    }
    else                                    //jesli nie wybrano jeszcze start
        if (event->button() == Qt::LeftButton)
            setBegin();

    update();
    QGraphicsItem::mousePressEvent(event);
}
//wyświetlają wspólrzędne sześcianu po nakierowaniu na niego kursora myszy
void Hex::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    QString c = "Współrzędne: "  + QString::number(x) + " , " + QString::number(y);
    emit showCoordinates(c);
    update();
}

void Hex::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    QString c = "Współrzędne: "  + QString::number(0) + " , " + QString::number(0);
    emit showCoordinates(c);
    update();
}


