#include "balance_profilecutting.h"
#include <QDebug>


balance_profilecutting::balance_profilecutting()
    : key    (new Key())
      , mill (new Mill())
{
    qDebug() << "Конструктор snakeCutting вызван";
}

void balance_profilecutting::balanceCutting(Key& key, Mill& mill, QVector<balanceCut>& cuts, coordSystem& cs)
{
    // начало ключа

    moveTo(1, 2);
}
