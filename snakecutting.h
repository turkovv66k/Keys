#pragma once

#include "Key.h"
#include "mill.h"

class snakeCutting
{
public:
    snakeCutting();


    void cutting(Mill, Key);
    void moveTo(double X, double Y, double Z);

private:


    // доп ширина площадки
    double  deltaD = 0.2;
    // толщина змейки
    double  Hzm = 2;
    // Угол заходной фаски
    int     bevelAngle = 30;
    // Глубина змейки считается от базы ( толщина ключа - толщина змейки )
    double  dept = K.Hkey - Hzm;

    Mill    M;
    Key     K;
    // вырезы

    struct snakeCut
    {
        // Расстояние от упорного торца до центра выреза
        double  B;
        // Ширина площадки на дне выреза
        double  D;
        // Расстояние от базового ребра (плоскости) до выреза
        double  L;
    };
};
