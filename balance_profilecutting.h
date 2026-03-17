#pragma once

#include "coordSystem.h"
#include "Key.h"
#include "mill.h"
#include "QVector"
#include <qscopedpointer.h>

class PathViewer;


class balance_profilecutting
{
public:
    balance_profilecutting();


    struct balanceCut
    {
        // Расстояние от упорного торца до начала выреза
        double  B = 0;
        //угол выреза
        int  angle = 0;
        //ширина выреза
        double D = 1.6;
    };



    void fillingCuttings();
    void balanceCutting(Key&, Mill&, QVector<balanceCut>&, coordSystem&);

private:

    void moveTo(double X, double Y, int Angle);

    QVector<balanceCut>  cuts;
    QScopedPointer<Key>   key;
    QScopedPointer<Mill>  mill;

};
