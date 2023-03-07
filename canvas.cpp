#include "canvas.h"

Canvas::Canvas()
{
    this->setFixedSize(600,400);
    this->setMouseTracking(true);
    this->setCursor(Qt::CrossCursor);

    gv=new GC();
    gv->setParent(this);
    gv->show();
}

void Canvas::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPen pen(QColor(100,100,100,50));
    pen.setWidth(5);
    painter.setPen(pen);
    QBrush brush(QColor(255,255,255));
    painter.setBrush(brush);
    QRect rect=this->rect();
    painter.drawRoundedRect(rect,0,0);
}

void Canvas::clear()
{
    gv->init();
}

void Canvas::resetcolor()
{
    gv->stopAnimation();
    gv->recoverlinesandvexes();
}




