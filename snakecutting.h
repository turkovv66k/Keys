#pragma once

#include "coordSystem.h"
#include "Key.h"
#include "mill.h"
#include "QVector"

class snakeCutting
{
public:
    snakeCutting();


    void cutting(Mill, Key);
    void moveTo(double X, double Y, double Z);

private:


    // доп ширина площадки
    double             deltaD = 0.2;
    // толщина змейки
    double             Hzm = 2;
    // Угол заходной фаски
    int                bevelAngle = 30;
    // Глубина змейки считается от базы ( толщина ключа - толщина змейки )
    double             Zdept = 1;

    Mill               mill;
    Key                key;
    coordSystem        cordS;

    // вырезы
    struct snakeCut
    {
        // Расстояние от упорного торца до центра выреза
        double  B;
        // Расстояние от базового ребра (плоскости) до выреза
        double  L;
        // Ширина площадки на дне выреза
        double  D;
    };

    QVector<snakeCut>  cuts;
};
