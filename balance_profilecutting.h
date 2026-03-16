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
        int  angle = 0;

        // Расстояние от упорного торца до центра выреза
        double  B = 0;
    };

    void fillingCuttings();
    void balanceCutting(Key&, Mill&, QVector<balanceCut>&, coordSystem&);

private:

    void moveTo(double X, double Y, int Angle);

    QScopedPointer<Key>   key;
    QScopedPointer<Mill>  mill;
    QVector<balanceCut>   cuts;
};
