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

// нарезка ширины равной ширины фрезы
void snakeCutting::singleCutting(Mill                          mill,
                                            Key                key,
                                            QVector<snakeCut>  cuts,
                                            bool               isBaseSupport,
                                            bool               isLeftSide,
                                            coordSystem        CS)
{
    // коэффициент расчёта X координаты если от левой стороны то 1 если от правой то -1
    int k = 1;

    if (isBaseSupport)
    {
        // заменяем L с кноца в начало если упор в торец и сбрасываем длину ключа key.L
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

    auto cutIndex = [&](int j) -> qsizetype {
        return isBaseSupport
               ? cuts.size() - j - 1
               : j;
    };

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
    snakeCutting::moveTo(CS.X0 + k * (maxL + mill.D / 2),
                         CS.Y0 + key.L + 2 * mill.D,
                         CS.Z0 + key.H + 2);

    int passes = (int)ceil(Zdept / mill.DeltaH);
    // 1 проход если ширина змейки - 1 диаметр фрезы
    if (mill.D == Hzm)
    {
        for (int i = 1; i <= passes; i++)
        {
            // опускаемся по z на высоту ключа - iый проход
            snakeCutting::moveTo(CS.X0 + k * (maxL + mill.D / 2),
                                 CS.Y0 + key.L + 2 * mill.D,
                                 CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept)); // минус 0.2 мм

            // Z не меняется во всем цикле ток Х и У
            for (int j = cuts.length() - 1;
                                             j >= 0;
                                             j--)
            {
                if (j > 0)
                {
                    // по Х тоже самое

                    snakeCutting::moveTo(CS.X0 + k * (cuts[cutIndex(j)].L + mill.D / 2),
                                         CS.Y0 + key.L - ((key.L - cuts[cutIndex(j)].B)
                                             - (cuts[cutIndex(j)].D / 2 + deltaD)),
                                         CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));

                    snakeCutting::moveTo(CS.X0 + k * (cuts[cutIndex(j)].L + mill.D / 2),
                                         CS.Y0 + key.L - ((key.L - cuts[cutIndex(j)].B)
                                             + (cuts[cutIndex(j)].D / 2 + deltaD)),
                                         CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));
                }

                // если вырез последний режем доп площадку равную D фрезы запасом
                if (j == 0)
                {
                    snakeCutting::moveTo(CS.X0 + k * (cuts[cutIndex(j)].L + mill.D / 2),
                                         CS.Y0 + key.L - ((key.L - cuts[cutIndex(j)].B)
                                             - (cuts[cutIndex(j)].D / 2 + deltaD)),
                                         CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));

                    snakeCutting::moveTo(CS.X0 + k * (cuts[cutIndex(j)].L + mill.D / 2),
                                         CS.Y0 + key.L - ((key.L - cuts[cutIndex(j)].B)
                                             + (cuts[cutIndex(j)].D / 2 + cuts[cutIndex(j)].D)),
                                         CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));

                    // подем фрезы вверх для перехода на новый заход
                    snakeCutting::moveTo(CS.X0 + k * (cuts[cutIndex(j)].L + mill.D / 2),
                                         CS.Y0 + key.L - ((key.L - cuts[cutIndex(j)].B)
                                             + (cuts[cutIndex(j)].D / 2 + cuts[cutIndex(j)].D)),
                                         CS.Z0 + key.H + 2);
                }
            }

            // едем в начало ключа
            snakeCutting::moveTo(CS.X0 + k * (maxL + mill.D / 2),
            CS.Y0 + key.L + 2 * mill.D,
            CS.Z0 + key.H + 2);
        }
    }
}

void snakeCutting::cutsFilling1()
{   // наполняем массив вырезами
    cuts1.clear();
    // cuts1.append({0, 7, 1});
    cuts1.append({10, 5, 1});
    cuts1.append({20, 7, 1});
    cuts1.append({30, 6, 1});
    // cuts1.append({25, 5, 1});
}

void snakeCutting::cutsFilling2()
{   // наполняем массив вырезами
    cuts2.clear();
    cuts2.append({5, 7, 1});
    cuts2.append({15, 5, 1});
    cuts2.append({25, 6, 1});
    // cuts2.append({35, 7, 1});
    // cuts2.append({27.5, 5, 1});
}

void snakeCutting::middleCuttingOut()
{
    for (int i = cuts2.length(); i--; i < 0)
    {
        cuts1[i].L - cuts2[i].L;
    }
}

void snakeCutting::multiCutting(Mill mill, Key key, bool isBaseSupport, coordSystem CS)
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
