#include "PathViewer.h"
#include <QPainter>

PathViewer::PathViewer(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(800, 600);
    setAutoFillBackground(true);
}

void PathViewer::addPoint(double x, double y, double z)
{
    Q_UNUSED(z);

    points.append(QPointF(x * scale, y * scale));
    update();   // запрос перерисовки
}

void PathViewer::clear()
{
    points.clear();
    update();
}

void PathViewer::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black, 2));

    for (int i = 1; i < points.size(); ++i)
    {
        painter.drawLine(points[i - 1], points[i]);
    }
}
