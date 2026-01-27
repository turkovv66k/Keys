#include "PathViewer.h"
#include <cmath>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>

PathViewer::PathViewer(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(800, 600);
    setMouseTracking(true);         // <<< важно
}

void PathViewer::addPoint(double x, double y, double z)
{
    points.append({x, y, z});
    update();

    #ifdef QT_DEBUG
        QApplication::processEvents();
    #endif
}

QPointF PathViewer::toScreen(const PathPoint& p) const
{
    double px = offsetX + p.x * scale;
    double py = height() - (offsetY + p.y * scale);

    return QPointF(px, py);
}

void PathViewer::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    p.setRenderHint(QPainter::Antialiasing);

    // ---------- СЕТКА ----------
    const double stepPx = gridStepMm * scale;

    int          cols = width() / stepPx + 2;
    int          rows = height() / stepPx + 2;

    for (int i = -cols; i < cols; ++i)
    {
        bool   bold = (i % boldEvery == 0);
        p.setPen(QPen(bold ? QColor(180, 180, 180) : QColor(220, 220, 220),
            bold ? 1.5 : 1.0));

        double x = offsetX + i * stepPx;
        p.drawLine(QPointF(x, 0), QPointF(x, height()));
    }

    for (int j = -rows; j < rows; ++j)
    {
        bool   bold = (j % boldEvery == 0);
        p.setPen(QPen(bold ? QColor(180, 180, 180) : QColor(220, 220, 220),
            bold ? 1.5 : 1.0));

        double y = height() - (offsetY + j * stepPx);
        p.drawLine(QPointF(0, y), QPointF(width(), y));
    }

    // ---------- ОСИ ----------
    p.setPen(QPen(Qt::black, 2));
    p.drawLine(offsetX, 0,                  offsetX, height()); // Y
    p.drawLine(0,       height() - offsetY, width(), height() - offsetY);   // X

    // ---------- ТРАЕКТОРИЯ ----------
    p.setPen(QPen(Qt::blue, 2));
    for (int i = 1; i < points.size(); ++i)
    {
        p.drawLine(toScreen(points[i - 1]), toScreen(points[i]));
    }

    // ---------- ТОЧКИ ----------
    p.setBrush(Qt::red);
    p.setPen(Qt::NoPen);
    for (const auto& pt : points)
    {
        p.drawEllipse(toScreen(pt), 3, 3);
    }
}

void PathViewer::mouseMoveEvent(QMouseEvent* event)
{
    constexpr double hitRadiusPx = 6.0;

    for (const auto& pt : points)
    {
        QPointF screenPt = toScreen(pt);
        double  dx = event->pos().x() - screenPt.x();
        double  dy = event->pos().y() - screenPt.y();

        if (std::sqrt(dx * dx + dy * dy) <= hitRadiusPx)
        {
            QToolTip::showText(event->globalPos(),
                    QString("X: %1\nY: %2\nZ: %3")
                    .    arg(pt.x, 0, 'f', 2)
                        .arg(pt.y, 0, 'f', 2)
                        .arg(pt.z, 0, 'f', 2),
                    this);
            return;
        }
    }

    QToolTip::hideText();
}
