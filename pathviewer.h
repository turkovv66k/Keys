#pragma once

#include <QPointF>
#include <QVector>
#include <QWidget>

class PathViewer : public QWidget
{
    Q_OBJECT

public:
    explicit PathViewer(QWidget* parent = nullptr);


    void addPoint(double x, double y, double z);
    void clear();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVector<QPointF>  points;
    double            scale = 1.0;
};
