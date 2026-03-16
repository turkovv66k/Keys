#include "balance_profilecutting.h"
#include <QDebug>


balance_profilecutting::balance_profilecutting()
    : key    (new Key())
      , mill (new Mill())
{
    qDebug() << "Конструктор snakeCutting вызван";
}

void balance_profilecutting::moveTo(double X, double Y, int Angle)
{
    qDebug() << "moveTo:" << "по X"<< X << "по Y"<< Y <<"угол поворота"<< Angle;
};

void balance_profilecutting::balanceCutting(Key& key, Mill& mill, QVector<balanceCut>& cuts, coordSystem& cs)
{

    // подезжаем к первому вырезу

    moveTo(1,2,3);
}





