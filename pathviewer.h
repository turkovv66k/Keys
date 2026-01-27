#pragma once
#include <QVector>
#include <QWidget>

struct PathPoint
{
    double  x;
    double  y;
    double  z;
};

class PathViewer : public QWidget
{
    Q_OBJECT

public:
    explicit PathViewer(QWidget* parent = nullptr);


    void    addPoint(double x, double y, double z);

protected:
    void    paintEvent(QPaintEvent*) override;
    void    mouseMoveEvent(QMouseEvent* event) override;

private:
    QVector<PathPoint>  points;

    // отображение
    double              scale = 10.0;
    double              offsetX = 100.0;
    double              offsetY = 100.0;

    double              gridStepMm = 1.0;
    int                 boldEvery = 10;

    QPointF toScreen(const PathPoint& p) const;
};
