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
        double  angle1 = 0;
        double  angle2 = 0;

        // Расстояние от упорного торца до центра выреза
        double  B = 0;
    };

    void balanceCutting(Key&, Mill&, QVector<balanceCut>&, coordSystem&);

private:

    void moveTo(double X, double Y);

    QScopedPointer<Key>   key;
    QScopedPointer<Mill>  mill;
};
