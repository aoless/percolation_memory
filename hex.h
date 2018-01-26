#ifndef HEX_H
#define HEX_H

#include <QObject>
#include <QPolygonF>
#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsSceneMouseEvent>
#include <QBrush>
#include <QString>
#include <QColor>
#include <QMouseEvent>
#include <QTime>
#include <QtCore>

//obiekt wyświetlany jako sześcian w oknie graficznym programu
class Hex : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
protected:
    enum MODE { empty, open, obstacle, begin, end };
private:
    QPolygonF hexagon;
    int x, y;
    int force;
    int forceClicked = 1;
    int strongest = 0;
    bool selectable = false;
    bool beginIsPressed = false;
    bool visited = false;
public:
    Hex(int x_ = 0, int y_ = 0);
    ~Hex();
    MODE mode;

    QPolygonF getHex() const { return hexagon; }
    QVector<int> findNeighbour(int d);

    bool endNeighbour(int nX, int nY);
    bool isVisited() { return visited; }

    void changeForceClick();
    void setToObstacle();
    void increaseForce();
    void setForceClicked(int f) { forceClicked = f; }
    void reset();
    void setBegin();
    void setEnd();
    void endIsSelectable() { beginIsPressed = true; }
    void isSelectable() { selectable = true; }

    int getX() { return x; }
    int getY() { return y; }
    int getForce() { return force; }

    void setNeighbour();
    void mousePressEvent(QGraphicsSceneMouseEvent *);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *);

    void setColor();
    void setToOpen();
    void setForce();
    void refresh();
    void percolate_open();
signals:
    void beginIsSet(int _x, int _y);
    void endIsSet(int _x, int _y);
    void showCoordinates(QString);
    void statement(QString);
    void start(QString);
    void stop(QString);

};

inline void delay(int msecs)
{
    QTime dieTime = QTime::currentTime().addMSecs(msecs);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents( QEventLoop::ExcludeUserInputEvents, 100 );
}


#endif // HEX_H
