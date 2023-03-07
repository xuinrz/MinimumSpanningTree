#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QRect>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include "graphcanvas.h"

class Canvas : public QWidget
{
    Q_OBJECT

private:
    void paintEvent(QPaintEvent *);

public:
    Canvas();
    GC *gv;

public slots:
    void clear();
    void resetcolor();
};


#endif // CANVAS_H
