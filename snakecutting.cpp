#include "PathViewer.h"
#include "snakecutting.h"
#include <algorithm>
#include <QDebug>

snakeCutting::snakeCutting()
{}


PathViewer* snakeCutting::pathViewer = nullptr;

void snakeCutting::moveTo(double X, double Y, double Z)
{
    qDebug() << "moveTo:" << X << Y << Z;
    if (pathViewer)
    {
        pathViewer->addPoint(X, Y, Z);
    }
}

void snakeCutting::setViewer(PathViewer* viewer)
{
    pathViewer = viewer;
}

void snakeCutting::cutsFilling1()
{   // наполняем массив вырезами
    cuts.clear();
    cuts.append({6.85, 4, 0.2});
    cuts.append({10.75, 4, 0.2});
    cuts.append({14.65, 6, 0.2});
    cuts.append({18.55, 7, 0.2});
    cuts.append({22.45, 8, 0.2});
}

void snakeCutting::cutting(Mill mill, Key key, bool isBaseSupport, bool isLeftSide)
{
    if (isLeftSide)
    {
        cutsFilling1();
    }
    else
    {
        cutsFilling1();
    }

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
            std::swap(cuts[i], cuts[j]);
            ++i;
            --j;
        }
    }

    if (!isLeftSide)
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
                    snakeCutting::moveTo(cordS.X0 + k * (cuts[cuts.size() - j - 1].L + mill.D / 2),
                                         cordS.Y0 + key.L
                                         - ((key.L - cuts[cuts.size() - j - 1].B)
                                             + (cuts[cuts.size() - j - 1].D + deltaD)),
                                         cordS.Z0 + key.H - mill.DeltaH * ((Zdept / mill.DeltaH) - i));
                    snakeCutting::moveTo(cordS.X0 + k * (cuts[cuts.size() - j - 1].L + mill.D / 2),
                                     cordS.Y0 + key.L
                                     - ((key.L - cuts[cuts.size() - j - 1].B) - (cuts[cuts.size() - j - 1].D + deltaD)),
                                     cordS.Z0 + key.H - mill.DeltaH * ((Zdept / mill.DeltaH) - i));
                }

                // если вырез последний режем доп площадку равную D фрезы запасом
                if (j == 0)
                {
                    snakeCutting::moveTo(cordS.X0 + k * (cuts[cuts.size() - j - 1].L + mill.D / 2),
                                         cordS.Y0 + key.L
                                         - ((key.L - cuts[cuts.size() - j - 1].B)
                                             + (cuts[cuts.size() - j - 1].D + deltaD)),
                                         cordS.Z0 + key.H - mill.DeltaH * ((Zdept / mill.DeltaH) - i));
                    snakeCutting::moveTo(cordS.X0 + k * (cuts[cuts.size() - j - 1].L + mill.D / 2),
                                         cordS.Y0 + key.L
                                         - ((key.L - cuts[cuts.size() - j - 1].B)
                                             - (cuts[cuts.size() - j - 1].D + deltaD)),
                                         cordS.Z0 + key.H - mill.DeltaH * ((Zdept / mill.DeltaH) - i));
                }
            }

            //// поднимаемся на безапосную высоту двигаемся только по Z вверх
            // snakeCutting::moveTo(cordS.X0 + k * (cuts.last().L + mill.D / 2),
            // cordS.Y0 + key.L - ((key.L - cuts.last().B) - cuts.last().D / 2 - mill.D),
            // cordS.Z0 + key.H + 2);
            //// едем в начало ключа
            // snakeCutting::moveTo(cordS.X0 + k * (cuts.last().L + mill.D / 2),
            // cordS.Y0 + key.L + 2 * mill.D,
            // cordS.Z0 + key.H + 2);
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
