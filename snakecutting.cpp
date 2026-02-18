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

double snakeCutting::interpolateL(const QVector<snakeCut>& cuts, double B)
{
    if (cuts.isEmpty())
    {
        return 0.0;
    }

    // Если вне диапазона — возвращаем край
    if (B >= cuts.first().B)
    {
        return cuts.first().L;
    }

    if (B <= cuts.last().B)
    {
        return cuts.last().L;
    }

    // Ищем сегмент ломаной
    for (int i = 0; i < cuts.size() - 1; ++i)
    {
        double B1 = cuts[i].B;
        double B2 = cuts[i + 1].B;

        if (((B1 >= B) && (B >= B2)) || ((B2 >= B) && (B >= B1)))
        {
            double L1 = cuts[i].L;
            double L2 = cuts[i + 1].L;

            if (std::abs(B1 - B2) < 1e-9)
            {
                return L1;
            }

            double t = (B - B2) / (B1 - B2);
            return L2 + t * (L1 - L2);
        }
    }

    // fallback (не должно происходить)
    return cuts.last().L;
}

void snakeCutting::doubleCutting(Mill&              mill,
                                 QVector<snakeCut>& cuts1,
                                 QVector<snakeCut>& cuts2,
                                 coordSystem&       CS1,
                                 coordSystem&       CS2,
                                 Key&               key,
                                 double             Zdept)
{
    // левая змейка
    singleCutting(mill, key, cuts1, false, true, CS1);
    CS1.X0 += mill.D;

    // правая змейка
    singleCutting(mill, key, cuts2, false, false, CS2);
    CS2.X0 -= mill.D;

    // зазор между змейками
    double xLeft = CS1.X0 + getMaxL(cuts1);
    double xRight = CS2.X0 - getMaxL(cuts2);
    double gap = xRight - xLeft;

    if (gap <= 0)
    {
        return;
    }

    int fillPasses = (int)ceil(gap / mill.D);
    fillInnerZone(mill, cuts1, cuts2, CS1, CS2, key, Zdept, fillPasses);
}

void snakeCutting::fillInnerZone(Mill&              mill,
                                 QVector<snakeCut>& cuts1,
                                 QVector<snakeCut>& cuts2,
                                 coordSystem&       CS1,
                                 coordSystem&       CS2,
                                 Key&               key,
                                 double             Zdept,
                                 int                fillPasses)
{
    int      passesZ = (int)ceil(Zdept / mill.DeltaH);

    double   xLeft = CS1.X0 + getMaxL(cuts1);
    double   xRight = CS2.X0 - getMaxL(cuts2);

    // Ystart — первый вырез с макс B из обоих массивов
    snakeCut firstCut1 = getFirstCut(cuts1);
    snakeCut firstCut2 = getFirstCut(cuts2);
    snakeCut firstCut = (firstCut1.B > firstCut2.B) ? firstCut1 : firstCut2;
    double   Ystart = CS1.Y0 + firstCut.B + firstCut.D / 2.0 + deltaD;

    // Yend — последний вырез с мин B из обоих массивов
    snakeCut lastCut1 = getLastCut(cuts1);
    snakeCut lastCut2 = getLastCut(cuts2);
    snakeCut lastCut = (lastCut1.B < lastCut2.B) ? lastCut1 : lastCut2;
    double   Yend = CS1.Y0 + lastCut.B - lastCut.D / 2.0 - lastCut.D;

    for (int p = 1; p <= passesZ; ++p)
    {
        double cutZ = CS1.Z0 + key.H - std::min(p * mill.DeltaH, Zdept);

        for (int i = 0; i < fillPasses; ++i)
        {
            double X = xLeft + i * mill.D + mill.D / 2.0;
            if (X > xRight - mill.D / 2.0)
            {
                X = xRight - mill.D / 2.0;
            }

            moveTo(X, Ystart + 2.0 * mill.D, CS1.Z0 + key.H + 2.0);
            moveTo(X, Ystart + 2.0 * mill.D, cutZ);

            if (i % 2 == 0)
            {
                moveTo(X, Ystart, cutZ);
                moveTo(X, Yend,   cutZ);
            }
            else
            {
                moveTo(X, Yend,   cutZ);
                moveTo(X, Ystart, cutZ);
            }
        }

        moveTo(xLeft, Ystart + 2.0 * mill.D, CS1.Z0 + key.H + 2.0);
    }
}

double snakeCutting::getGlobalMaxB(const QVector<snakeCut>& cuts1, const QVector<snakeCut>& cuts2)
{
    double maxB = cuts1[0].B;

    for (const auto& c : cuts1)
    {
        maxB = std::max(maxB, c.B);
    }

    for (const auto& c : cuts2)
    {
        maxB = std::max(maxB, c.B);
    }

    return maxB;
}

double snakeCutting::getGlobalMinB(const QVector<snakeCut>& cuts1, const QVector<snakeCut>& cuts2)
{
    double minB = cuts1[0].B;

    for (const auto& c : cuts1)
    {
        minB = std::min(minB, c.B);
    }

    for (const auto& c : cuts2)
    {
        minB = std::min(minB, c.B);
    }

    return minB;
}

double snakeCutting::getMaxL(const QVector<snakeCut>& cuts)
{
    double maxL = cuts[0].L;

    for (const auto& c : cuts)
    {
        maxL = std::max(maxL, c.L);
    }

    return maxL;
}

void snakeCutting::cutsFilling1()
{   // наполняем массив вырезами
    cuts1.clear();

    cuts1.append({10, 5, 1});
    cuts1.append({20, 7, 1});
    cuts1.append({25, 5, 1});
    cuts1.append({30, 6, 1});
}

void snakeCutting::cutsFilling2()
{   // наполняем массив вырезами
    cuts2.clear();
    cuts2.append({5, 7, 1});
    cuts2.append({15, 5, 1});
    cuts2.append({25, 6, 1});
    cuts2.append({27.5, 5, 1});
    cuts2.append({35, 7, 1});
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
