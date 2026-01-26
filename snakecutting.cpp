#include "PathViewer.h"
#include "snakecutting.h"
#include <algorithm>
#include <QDebug>

snakeCutting::snakeCutting()
{}


PathViewer* snakeCutting::pathViewer = nullptr;

void snakeCutting::moveTo(double X, double Y, double Z)
{
    if (pathViewer)
    {
        pathViewer->addPoint(X, Y, Z);
    }
}

void snakeCutting::setViewer(PathViewer* viewer)
{
    pathViewer = viewer;
}

void snakeCutting::cutsFilling()
{   // наполняем массив вырезами
    cuts.append({15, 3, 2});
    cuts.append({25, 4, 3});
    cuts.append({35, 5, 2});
}

void snakeCutting::cutting(Mill mill, Key key, bool isBaseSupport = true, bool isRightSide = true)
{
    cutsFilling();
    // коэффициент расчёта X координаты если от левой стороны то 1 если от правой то -1
    int k = 1;

    if (isBaseSupport)
    {
        // заменяем L с кноца в начало если упор в торец и сбрасываем длину ключа key.L
        key.L = 0;

        int i = 0;
        int j = cuts.size() - 1;
        while (i < j)
        {
            std::swap(cuts[i].L, cuts[j].L);
            ++i;
            --j;
        }
    }

    if (isRightSide)
    {
        k = -1;
    }

    // подъезжаем к ключу (пока хз с + и -)
    snakeCutting::moveTo(cordS.X0 + k * (cuts.last().L + mill.D / 2),
                         cordS.Y0 + key.L + 2 * mill.D,
                         cordS.Z0 + key.H + 2);

    // 1 проход если ширина змейки  до 1 диаметра фрезы
    if ((mill.D <= Hzm) && (Hzm <= 2 * mill.D))
    {
        for (int i = Zdept / mill.DeltaH; i >= 0; i--)  // TO DO сделать проверку если глубина не делится на кол во
                                                        // проходов
        {
            // опускаемся по z на высоту ключа - iый проход
            snakeCutting::moveTo(cordS.X0 + k * (cuts.last().L + mill.D / 2),
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
                    snakeCutting::moveTo(cordS.X0 + k * (cuts[j].L + mill.D / 2),
                                     cordS.Y0 + key.L - ((key.L - cuts[j].B) - cuts[j].D / 2 - deltaD),
                                     cordS.Z0 + key.H - mill.DeltaH * ((Zdept / mill.DeltaH) - i));
                }

                // если вырез последний режем доп площадку равную D фрезы запасом
                if (j == 0)
                {
                    snakeCutting::moveTo(cordS.X0 + k * (cuts[j].L + mill.D / 2),
                                         cordS.Y0 + key.L - ((key.L - cuts[j].B) - cuts[j].D / 2 - mill.D),
                                         cordS.Z0 + key.H - mill.DeltaH * ((Zdept / mill.DeltaH) - i));
                }
            }

            // поднимаемся на безапосную высоту двигаемся только по Z вверх
            snakeCutting::moveTo(cordS.X0 + k * (cuts.first().L + mill.D / 2),
                                 cordS.Y0 + key.L - ((key.L - cuts.first().B) - cuts.first().D / 2 - mill.D),
                                 cordS.Z0 + key.H + 2);
            // едем в начало ключа
            snakeCutting::moveTo(cordS.X0 + k * (cuts.last().L + mill.D / 2),
                                 cordS.Y0 + key.L + 2 * mill.D,
                                 cordS.Z0 + key.H + 2);
        }
    }
    else
    {   // несколько проходов если толщина больше диаметра фрезы и  меньше ширины ключа - 2 мм (2 мм условно) ((проверка
        // на дурочка))
        if ((Hzm > mill.D) && (Hzm < key.W - 2))
        {
            for (int i = Hzm / mill.D; i > 0; i--)
            {
                for (int j = Zdept / mill.DeltaH; j > 0; j++)
                {}
            }
        }
    }
}
