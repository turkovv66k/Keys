#include "PathViewer.h"
#include "snakecutting.h"
#include <algorithm>
#include <math.h>
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
    cuts.append({6.85, 4, 1});
    cuts.append({10.75, 5, 1});
    cuts.append({14.65, 6, 1});
    cuts.append({18.55, 7, 1});
    cuts.append({22.45, 5.4, 1});
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
    // коэффициент расчёта Y координаты если упор в базу  то 1 если в торец то -1
    int n = 1;
    if (isBaseSupport)
    {
        // заменяем L с кноца в начало если упор в торец и сбрасываем длину ключа key.L

        n = -1;
        int i = 0;
        int j = cuts.size() - 1;
        while (i < j)
        {
            std::swap(cuts[i], cuts[j]);
            ++i;
            --j;
        }

        int k = 0;
        while (k < cuts.length())
        {
            cuts[k].B -= key.L;
            ++k;
        }

        key.L = 0;
    }

    // ищим крайний вырез в массиве
    double maxB = cuts.first().B;
    double maxL = cuts.first().L;
    // счетчик крайнего выреза
    int    maxI = 0;
    for (const auto& cut : qAsConst(cuts))
    {
        if (cut.B > maxB)
        {
            maxI++;
            maxB = cut.B;
            maxL = cut.L;
        }
    }

    if (!isLeftSide)
    {
        k = -1;
    }

    // подъезжаем к ключу (пока хз с + и -)
    snakeCutting::moveTo(cordS.X0 + k * (maxL + mill.D / 2),
                         cordS.Y0 + key.L + 2 * mill.D,
                         cordS.Z0 + key.H + 2);

    int passes = (int)ceil(Zdept / mill.DeltaH);
    // 1 проход если ширина змейки - 1 диаметр фрезы
    if (mill.D == Hzm)
    {
        for (int i = 1; i <= passes; i++)
        {
            // опускаемся по z на высоту ключа - iый проход
            snakeCutting::moveTo(cordS.X0 + k * (maxL + mill.D / 2),
                                 cordS.Y0 + key.L + 2 * mill.D,
                                 cordS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));  // минус 0.2 мм

            // Z не меняется во всем цикле ток Х и У
            for (int j = cuts.length() - 1;
                                             j >= 0;
                                             j--)
            {
                if (j > 0)
                {
                    // по Х тоже самое

                    snakeCutting::moveTo(cordS.X0 + k * (cuts[cuts.size() - j - 1].L + mill.D / 2),
                                         cordS.Y0 + key.L - ((key.L - cuts[cuts.size() - j - 1].B)
                                             + n * (cuts[cuts.size() - j - 1].D / 2 + deltaD)),
                                         cordS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));

                    snakeCutting::moveTo(cordS.X0 + k * (cuts[cuts.size() - j - 1].L + mill.D / 2),
                                         cordS.Y0 + key.L - ((key.L - cuts[cuts.size() - j - 1].B)
                                             - n * (cuts[cuts.size() - j - 1].D / 2 + deltaD)),
                                         cordS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));
                }

                // если вырез последний режем доп площадку равную D фрезы запасом
                if (j == 0)
                {
                    snakeCutting::moveTo(cordS.X0 + k * (cuts[cuts.size() - j - 1].L + mill.D / 2),
                                         cordS.Y0 + key.L - ((key.L - cuts[cuts.size() - j - 1].B)
                                             + n * (cuts[cuts.size() - j - 1].D / 2 + deltaD)),
                                         cordS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));

                    snakeCutting::moveTo(cordS.X0 + k * (cuts[cuts.size() - j - 1].L + mill.D / 2),
                                         cordS.Y0 + key.L - ((key.L - cuts[cuts.size() - j - 1].B)
                                             - n * (cuts[cuts.size() - j - 1].D / 2 + deltaD)),
                                         cordS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));
                    // подем фрезы вверх для перехода на новый заход
                    snakeCutting::moveTo(cordS.X0 + k * (cuts[cuts.size() - j - 1].L + mill.D / 2),
                                         cordS.Y0 + key.L - ((key.L - cuts[cuts.size() - j - 1].B)
                                             - n * (cuts[cuts.size() - j - 1].D / 2 + deltaD)),
                                         cordS.Z0 + key.H + 2);
                }
            }

            // едем в начало ключа
            snakeCutting::moveTo(cordS.X0 + k * (maxL + mill.D / 2),
                                 cordS.Y0 + key.L + 2 * mill.D,
                                 cordS.Z0 + key.H + 2);

            // TODO сделать в конце змейки зарез на mill.D /2
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
