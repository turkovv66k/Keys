#pragma once

#include "coordSystem.h"
#include "Key.h"
#include "mill.h"
#include "QVector"
#include <qscopedpointer.h>

class PathViewer;

class snakeCutting
{
public:
    snakeCutting();



    // вырезы
    struct snakeCut
    {
        // Расстояние от упорного торца до центра выреза
        double  B = 0;
        // Расстояние от базового ребра (плоскости) до выреза
        double  L = 0;
        // Ширина площадки на дне выреза
        double  D = 0;
    };

    QVector<snakeCut>  cuts1;
    QVector<snakeCut>  cuts2;
    // отрисовка
    static void                     setViewer(PathViewer* viewer);

    void                            singleCutting(Mill&      mill,
                                          Key&               key,
                                          QVector<snakeCut>& snakeCuts,
                                          bool               isBaseSupport,
                                          bool               isLeftSide,
                                          coordSystem&       cs);

    void                            doubleCutting(Mill& mill,
        QVector<snakeCut>&                              cuts1,
        QVector<snakeCut>&                              cuts2,
        coordSystem&                                    CS1,
        coordSystem&                                    CS2,
        Key&                                            key,
        double                                          Zdept,
        bool                                            isBaseSupport);

    // void                            multiCutting(Mill, Key, bool, coordSystem);

    void                            cutsFilling1();
    void                            cutsFilling2();

private:
    // метод который возвращает массив для нарезки от базы или от вставки  (отнимает от вырезов длину ключа и ставит
    // длину ключа 0 )
    QVector<snakeCutting::snakeCut> offsetCuts(QVector<snakeCut>& cuts, Key& key);

    // метод передвижения по координатам
    void                            moveTo(double X, double Y, double Z);

    snakeCut                        getFirstCut(QVector<snakeCut>& cuts);

    snakeCut                        getLastCut(QVector<snakeCut>& cuts);

    double                          interpolateL(QVector<snakeCut>& cuts, double B);

    double                          getMaxL(const QVector<snakeCut>& cuts);

    snakeCut                        getGlobalMinCut(const QVector<snakeCut>& cuts1, const QVector<snakeCut>& cuts2);

    snakeCut                        getGlobalMaxCut(const QVector<snakeCut>& cuts1, const QVector<snakeCut>& cuts2);

    void                            fillInnerZone(Mill& mill,
                                     QVector<snakeCut>& cuts1,
                                     QVector<snakeCut>& cuts2,
                                     coordSystem&       CS1,
                                     coordSystem&       CS2,
                                     Key&               key,
                                     double             Zdept,
                                     int                fillPasses);


    // коэффициент расчёта X координаты если от левой стороны то 1 если от правой то -1
    int                             getCutsSide(bool);


    // отрисовка
    static PathViewer*    pathViewer;

    // доп ширина площадки
    double                deltaD = 0.2;
    // толщина змейки
    double                Hzm = 2;
    // Угол заходной фаски
    int                   bevelAngle = 30;
    // Глубина змейки считается от базы
    double                Zdept = 0.2;

    QScopedPointer<Key>   key;
    QScopedPointer<Mill>  mill;
};
