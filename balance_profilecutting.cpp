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

void balance_profilecutting::balanceCutting(Key& key, Mill& mill, QVector<balanceCut>& cuts, coordSystem& CS)
{
    //заполняем массив данными
    void fillingCuttings();

    //условное безопасное расстояние до заготовки
    double Xbez = 30;

    // подезжаем к первому вырезу
    moveTo(CS.X0 + Xbez, CS.Y0, 0);

    //проезжаем до 1 действительного выреза
    moveTo(CS.X0 + Xbez, CS.Y0 + , 0);

}



void balance_profilecutting::fillingCuttings()
{
    cuts.append({0, 0});//1
    cuts.append({1.6, 0});//2
    cuts.append({3.2, 0});//3
    cuts.append({5, 0});//4
    cuts.append({5, 0});//5
    cuts.append({5, 0});//6
    cuts.append({5, 0});//7
    cuts.append({5, 0});//8
    cuts.append({5, 0});//9
    cuts.append({5, 0});//10
    cuts.append({5, 0});//11
}

