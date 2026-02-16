#include "PathViewer.h"
#include "snakecutting.h"
#include <algorithm>
#include <math.h>
#include <QDebug>


snakeCutting::snakeCutting()
    : key    (new Key())
      , mill (new Mill())
{
    qDebug() << "Конструктор snakeCutting вызван";
}

PathViewer* snakeCutting::pathViewer = nullptr;

void snakeCutting::moveTo(double X, double Y, double Z)
{
    // qDebug() << "moveTo:" << X << Y << Z;
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
void snakeCutting::singleCutting(Mill&               mill,
                                  Key&               key,
                                  QVector<snakeCut>& cuts,
                                  bool               isBaseSupport,
                                  bool               isLeftSide,
                                  coordSystem&       CS)
{
    if (isBaseSupport)
    {
        // заменяем L с кноца в начало если упор в торец и сбрасываем длину ключа key.L
        cuts = offsetCuts(cuts, key);
    }

    // находим самый первый вырез для начала резки
    auto   maxCut = getFirstCut(cuts);
    double maxL = maxCut.L;

    int    k = getCutsSide(isLeftSide);

    // подъезжаем к ключу
    snakeCutting::moveTo(CS.X0 + k * (maxL + mill.D / 2),
                         CS.Y0 + key.L + 2 * mill.D,
                         CS.Z0 + key.H + 2);

    int passes = (int)ceil(Zdept / mill.DeltaH);

    for (int i = 1; i <= passes; i++)
    {
        // опускаемся по z на высоту ключа - iый проход
        snakeCutting::moveTo(CS.X0 + k * (maxL + mill.D / 2),
                                CS.Y0 + key.L + 2 * mill.D,
                                CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));  // минус 0.2 мм
        // Z не меняется во всем цикле ток Х и У
        for (int j = cuts.length() - 1;
                                            j >= 0;
                                            j--)
        {
            if (j > 0)
            {
                // по Х тоже самое
                snakeCutting::moveTo(CS.X0 + k * (cuts[j].L + mill.D / 2),
                                        CS.Y0 + key.L - ((key.L - cuts[j].B)
                                            - (cuts[j].D / 2 + deltaD)),
                                        CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));
                snakeCutting::moveTo(CS.X0 + k * (cuts[j].L + mill.D / 2),
                                        CS.Y0 + key.L - ((key.L - cuts[j].B)
                                            + (cuts[j].D / 2 + deltaD)),
                                        CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));
            }

            // если вырез последний режем доп площадку равную D фрезы запасом
            if (j == 0)
            {
                snakeCutting::moveTo(CS.X0 + k * (cuts[j].L + mill.D / 2),
                                        CS.Y0 + key.L - ((key.L - cuts[j].B)
                                            - (cuts[j].D / 2 + deltaD)),
                                        CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));
                snakeCutting::moveTo(CS.X0 + k * (cuts[j].L + mill.D / 2),
                                        CS.Y0 + key.L - ((key.L - cuts[j].B)
                                            + (cuts[j].D / 2 + cuts[j].D)),
                                        CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));
                // подем фрезы вверх для перехода на новый заход
                snakeCutting::moveTo(CS.X0 + k * (cuts[j].L + mill.D / 2),
                                        CS.Y0 + key.L - ((key.L - cuts[j].B)
                                            + (cuts[j].D / 2 + cuts[j].D)),
                                        CS.Z0 + key.H + 2);
            }
        }

        // едем в начало ключа
        snakeCutting::moveTo(CS.X0 + k * (maxL + mill.D / 2),
           CS.Y0 + key.L + 2 * mill.D,
           CS.Z0 + key.H + 2);
    }
}

QVector<snakeCutting::snakeCut> snakeCutting::offsetCuts(QVector<snakeCut>& cuts, Key& key)
{
    if (cuts.isEmpty())
    {
        return cuts;
    }

    int f = 0;
    while (f < cuts.length())
    {
        cuts[f].B -= key.L;
        ++f;
    }

    key.L = 0;

    return cuts;
}

snakeCutting::snakeCut snakeCutting::getFirstCut(QVector<snakeCut>& cuts)
{
    snakeCut maxCut;
    // счетчик крайнего выреза
    int      maxI = 0;

    maxCut.B = cuts.first().B;
    maxCut.L = cuts.first().L;
    for (const auto& cut : qAsConst(cuts))
    {
        if (cut.B > maxCut.B)
        {
            maxI++;
            maxCut.B = cut.B;
            maxCut.L = cut.L;
        }
    }

    return maxCut;
}

snakeCutting::snakeCut snakeCutting::getLastCut(QVector<snakeCut>& cuts)
{
    snakeCut minCut;

    minCut.B = cuts.first().B;
    minCut.L = cuts.first().L;
    for (const auto& cut : qAsConst(cuts))
    {
        if (cut.B < minCut.B)
        {
            minCut.B = cut.B;
            minCut.L = cut.L;
        }
    }

    return minCut;
}

int snakeCutting::getCutsSide(bool  isLeftSide)
{
    return isLeftSide ? 1 : -1;
}

double snakeCutting::interpolateL(QVector<snakeCut>& cuts, double B)
{
    if (cuts.isEmpty())
    {
        return 0;
    }

    if (B <= cuts.first().B)
    {
        return cuts.first().L;
    }

    if (B >= cuts.last().B)
    {
        return cuts.last().L;
    }

    for (int i = 0; i < cuts.size() - 1; i++)
    {
        const snakeCut& c0 = cuts[i];
        const snakeCut& c1 = cuts[i + 1];
        if ((B >= c0.B) && (B <= c1.B))
        {
            double t = (B - c0.B) / (c1.B - c0.B);
            return c0.L + t * (c1.L - c0.L);
        }
    }

    return cuts.last().L;
}

void snakeCutting::doubleCutting(Mill& mill, Key& key1, Key& key2, coordSystem& cs1, coordSystem& cs2)
{
    singleCutting(mill, key1, cuts1, false, true,  cs1);
    singleCutting(mill, key2, cuts2, false, false, cs2);
}

void snakeCutting::betweenCutting(Mill& mill, coordSystem& cs1, coordSystem& cs2)
{
    double B_start = std::min(getLastCut(cuts1).B, getLastCut(cuts2).B) - mill.D / 2;
    double B_end = std::max(getFirstCut(cuts1).B, getFirstCut(cuts2).B) + mill.D / 2 + 2.0;

    if (B_start >= B_end)
    {
        return;
    }

    int    passes = (int)ceil(Zdept / mill.DeltaH);
    double X_start = cs1.X0 + (interpolateL(cuts1, B_start) + mill.D / 2);

    moveTo(X_start, cs1.Y0 + B_start, cs1.Z0 + 2);

    for (int i = 1; i <= passes; i++)
    {
        double Z = cs1.Z0 - std::min(i * mill.DeltaH, Zdept);

        moveTo(X_start, cs1.Y0 + B_start, Z);

        bool   goingRight = true;

        for (double B = B_start; B <= B_end; B += mill.D / 2)
        {
            double Y = cs1.Y0 + B;
            double X_left = cs1.X0 + (interpolateL(cuts1, B) + mill.D / 2);
            double X_right = cs2.X0 - (interpolateL(cuts2, B) + mill.D / 2);

            if (X_left >= X_right)
            {
                continue;
            }

            if (goingRight)
            {
                moveTo(X_left,  Y, Z);
                moveTo(X_right, Y, Z);
            }
            else
            {
                moveTo(X_right, Y, Z);
                moveTo(X_left,  Y, Z);
            }

            goingRight = !goingRight;
        }

        moveTo(X_start, cs1.Y0 + B_start, cs1.Z0 + 2);
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

// void snakeCutting::multiCutting(Mill mill, Key key, bool isBaseSupport, coordSystem CS)
// {   // несколько проходов если толщина больше диаметра фрезы и  меньше ширины ключа - 2 мм (2 мм условно) ((проверка
//// на дурочка))
// if ((Hzm > mill.D) && (Hzm < key.W - 2))
// {
// for (int i = Hzm / mill.D; i > 0; i--)
// {
// for (int j = Zdept / mill.DeltaH; j > 0; j++)
// {}
// }
// }
// }
