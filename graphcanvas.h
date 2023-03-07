#ifndef GRAPHCANVAS_H
#define GRAPHCANVAS_H

#include <QWidget>
#include <QMouseEvent>

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>

#include <QTimeLine>

#include <QVector>
#include <QStack>
#include <QQueue>
#include <QInputDialog>
#include <QMessageBox>
#include <Queue>


class GC;
class Vex;
class Line;


class GC : public QGraphicsView
{
    Q_OBJECT

private:
    QGraphicsScene* myGraphicsScene;
    QBrush regBrush = QBrush(QColor(108,100,205));

    int vexID = 0;//点数量
    bool isCreating = false;//
    Vex *strtVex = nullptr;
    QGraphicsItem *sketchItem = nullptr;

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    Vex* addVex(QPointF center, qreal radius = 10);
    Line* addLine(Vex *start, Vex *end);
    void clearSketch();
    void sketchLine(QPointF start, QPointF end);

    /* Animation loop */
    QQueue<QTimeLine*> aniQueue;
    bool onAni = false;
    QTimeLine *curAni = nullptr;
    void nextAni();
    void addAnimation(QTimeLine *ani);

    std::vector<std::vector<int>> matrix;
    QVector<Vex*> vexes;//storing all nodes
    QVector<Line*> lines;
    QVector<Line*> coverlines;
    QVector<Vex*> preVexes;//node that has child
    QVector<Vex*> leaves;
    QVector<Vex*> halfLeaves;//node that only has one child
    QVector<Vex*> nullVexes;//nullptr
    QVector<Line*> leafLines;//between a leaf node and a nullptr

public:
    qreal speedRate = 0.5;
    GC();

    Vex * root;

    void stopAnimation();
    void recoverlinesandvexes();
    bool buildMatrix();
    void kruskal(double speed, QVector<QLineEdit*> es);
    void prim(double speed);
    void prim2(double speed, QVector<QLineEdit*> es);
    QTimeLine* coverline(Vex *start, Vex *end, QColor color);
    QTimeLine* hightlightcode(QLineEdit* e);

    void init();
};


class Vex : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

private:
    QBrush regBrush = QBrush(QColor(108,100,205));
    QBrush visitedBrush = QBrush(QColor(160,200,100));
    QFont nameFont = QFont("Corbel", 13, QFont::Normal, true);

    QTimeLine* curAnimation = nullptr;
    void startAnimation();

public:
    QPointF center;
    qreal radius;

    QVector<Vex*> nexts;//邻居节点
    Vex *left = nullptr;//左节点
    Vex *right = nullptr;//右节点

    /* For display tag */
    QGraphicsSimpleTextItem *nameTag = nullptr;
    QString nameText = "";
    void setName(QString s);

    Vex(QPointF _center, qreal _r, int nameID = 0, QGraphicsItem *parent = nullptr);
    Vex(QPointF _center, qreal _r=5, QGraphicsItem *parent = nullptr);

    void showAnimation();//运行动画

    QTimeLine* visit();
};


class Line : public QObject, public QGraphicsLineItem{
    Q_OBJECT

private:
    /* detail of the line */
    int weight;
    qreal lineWidth = 5;
    Qt::PenStyle lineStyle = Qt::SolidLine;
    Qt::PenCapStyle capStyle = Qt::RoundCap;
    QColor defaultColor = QColor(159,182,205);
    QPen defaultPen;

    /* basic data */
    Vex *startVex;
    Vex *endVex;

public:
    Line(Vex *start, Vex *end, QGraphicsItem *parent = nullptr);
    Line(Vex *start, Vex *end, int weight, QGraphicsItem *parent = nullptr);
    Line(Vex *start, Vex *end, QColor q, QGraphicsItem *parent = nullptr);
    int getweight(){return weight;}
    Vex * getstart(){return startVex;}
    Vex * getend(){return endVex;}
    QTimeLine* visit();
};
#endif // GRAPHCANVAS_H
