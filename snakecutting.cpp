#include "snakecutting.h"
#include <algorithm>
#include <QDebug>

snakeCutting::snakeCutting()
{}

// Метод передвижения по координатам (заглушка)
// X ось x, Y ось y, Z ось z
void snakeCutting::moveTo(double X, double Y, double Z)
{
    qDebug() << "X:" << X << "Y:" << Y << "Z:" << Z;
    qDebug() << "----------------------";
}

void snakeCutting::cutsFilling()
{   // наполняем массив вырезами
    cuts.append({15, 3, 2});
    cuts.append({25, 4, 3});
    cuts.append({35, 5, 2});
}

void snakeCutting::cutting(Mill mill, Key key, bool isBaseSupport = false)
{
    cutsFilling();
    if (true)
    {
        key.L = 0;
        std::reverse(cuts.begin(), cuts.end());
    }

    // подъезжаем к ключу (пока хз с + и -)
    snakeCutting::moveTo(cordS.X0 + cuts.last().L + mill.D / 2,
                         cordS.Y0 + key.L + 2 * mill.D,
                         cordS.Z0 + key.H + 2);

    // 1 проход если ширина змейки от 1 до 2 диаметров фрезы
    if ((mill.D <= Hzm) && (Hzm <= 2 * mill.D))
    {
        for (int i = Zdept / mill.DeltaH; i >= 0; i--)  // TO DO сделать проверку если глубина не делится на кол во
                                                        // проходов
        {
            // опускаемся по z на высоту ключа - iый проход
            snakeCutting::moveTo(cordS.X0 + cuts.last().L + mill.D / 2,
                                 cordS.Y0 + key.L + 2 * mill.D,
                                 cordS.Z0 + key.H - mill.DeltaH * ((Zdept / mill.DeltaH) - i)); // минус 0.2 мм

            // Z не меняется во всем цикле ток Х и У
            for (int j = cuts.length() - 1;
                                             j >= 0;
                                             j--)
            {
                if (j > 0)
                {
                    // по Х тоже самое
                    snakeCutting::moveTo(cordS.X0 + cuts[j].L + mill.D / 2,
                                     cordS.Y0 + key.L - ((key.L - cuts[j].B) - cuts[j].D / 2 - deltaD),
                                     cordS.Z0 + key.H - mill.DeltaH * ((Zdept / mill.DeltaH) - i));
                }

                // если вырез последний режем доп площадку равную D фрезы запасом
                if (j == 0)
                {
                    snakeCutting::moveTo(cordS.X0 + cuts[j].L + mill.D / 2,
                                         cordS.Y0 + key.L - ((key.L - cuts[j].B) - cuts[j].D / 2 - mill.D),
                                         cordS.Z0 + key.H - mill.DeltaH * ((Zdept / mill.DeltaH) - i));
                }
            }

            // поднимаемся на безапосную высоту двигаемся только по Z вверх
            snakeCutting::moveTo(cordS.X0 + cuts.first().L + mill.D / 2,
                                 cordS.Y0 + key.L - ((key.L - cuts.first().B) - cuts.first().D / 2 - mill.D),
                                 cordS.Z0 + key.H + 2);
            // едем в начало ключа
            snakeCutting::moveTo(cordS.X0 + cuts.last().L + mill.D / 2,
                                 cordS.Y0 + key.L + 2 * mill.D,
                                 cordS.Z0 + key.H + 2);
        }
    }
    else
    {   // несколько проходов если толщина больше двух диаметров фрезы и меньше ширины ключа - 2 мм (2 мм условно)
        if ((Hzm >= 2 * mill.D) && (Hzm < key.W - 2))
        {
            // for ()
            // {}
        }
    }
}
