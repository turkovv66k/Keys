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
                                            + (cuts[j].D / 2) - deltaD * 5),    ////
                                        CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept));
                // подем фрезы вверх для перехода на новый заход
                snakeCutting::moveTo(CS.X0 + k * (cuts[j].L + mill.D / 2),
                                        CS.Y0 + key.L - ((key.L - cuts[j].B)
                                            + (cuts[j].D / 2) - deltaD * 5),    ////
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

void snakeCutting::doubleCutting(Mill&              mill,
                                 QVector<snakeCut>& cuts1,
                                 QVector<snakeCut>& cuts2,
                                 coordSystem&       CS1,
                                 coordSystem&       CS2,
                                 Key&               key,
                                 double             Zdept)
{
    int    passesZ = (int)ceil(Zdept / mill.DeltaH);
    double shift1 = +mill.D * 0.9;
    double shift2 = -mill.D * 0.9;
    double stepB = mill.D * 0.9;    // шаг между строчками зачистки

    auto   snakePass = [&](QVector<snakeCut>& cuts, coordSystem& CS,
                         int kSide, double shift, double cutZ){
        auto   firstCut = getFirstCut(cuts);
        double maxL = firstCut.L;

        moveTo(CS.X0 + kSide * (maxL + mill.D / 2),
               CS.Y0 + key.L + 2 * mill.D,
               cutZ);

        for (int j = cuts.length() - 1; j >= 0; j--)
        {
            double X = CS.X0 + kSide * (cuts[j].L + mill.D / 2);
            double Y1 = CS.Y0 + cuts[j].B + cuts[j].D / 2.0 + deltaD;
            double Y2 = CS.Y0 + cuts[j].B - cuts[j].D / 2.0 - deltaD;
            moveTo(X, Y1, cutZ);
            moveTo(X, Y2, cutZ);
            if (j == 0)
            {
                double Ybot = CS.Y0 + cuts[j].B - cuts[j].D / 2.0 - deltaD * 5;
                moveTo(X,         Ybot, cutZ);
                moveTo(X + shift, Ybot, cutZ);
            }
        }

        for (int j = 0; j < cuts.length(); j++)
        {
            double X = CS.X0 + kSide * (cuts[j].L + mill.D / 2) + shift;
            double Y2 = CS.Y0 + cuts[j].B - cuts[j].D / 2.0 - deltaD;
            double Y1 = CS.Y0 + cuts[j].B + cuts[j].D / 2.0 + deltaD;
            moveTo(X, Y2, cutZ);
            moveTo(X, Y1, cutZ);
            if (j == cuts.length() - 1)
            {
                double Ysafe = CS.Y0 + key.L + 2 * mill.D;
                moveTo(X, Ysafe, cutZ);
                moveTo(X, Ysafe, CS.Z0 + key.H + 2);
            }
        }
    };

    auto betweenPass = [&](double cutZ){
        snakeCut    minCut = getGlobalMinCut(cuts1, cuts2);
        double      B_start = key.L;
        double      B_end = minCut.B - deltaD * 5 - minCut.D / 2;
        bool        goRight = true;

        coordSystem CS1s = CS1;
        coordSystem CS2s = CS2;
        CS1s.X0 += shift1;
        CS2s.X0 += shift2;

        auto        doRow = [&](double B){
            double Y = CS1.Y0 + B;
            double X_left = CS1s.X0 + (interpolateL(cuts1, B) + mill.D / 2.0) + stepB;
            double X_right = CS2s.X0 - (interpolateL(cuts2, B) + mill.D / 2.0) - stepB;

            if (X_left >= X_right)
            {
                return;
            }

            if (goRight)
            {
                moveTo(X_left,  Y, cutZ);
                moveTo(X_right, Y, cutZ);
            }
            else
            {
                moveTo(X_right, Y, cutZ);
                moveTo(X_left,  Y, cutZ);
            }

            goRight = !goRight;
        };

        double B = B_start;
        while (B > B_end)
        {
            doRow(B);
            B -= stepB;
            if (B < B_end)
            {
                B = B_end;  // последний проход точно на B_end
            }
        }

        doRow(B_end);
    };

    double Zsafe = CS1.Z0 + key.H + 2.0;

    moveTo(CS1.X0 + getMaxL(cuts1) + mill.D / 2, CS1.Y0 + key.L + 2 * mill.D, Zsafe);

    for (int p = 1; p <= passesZ; (((((((((((((((((((((((((((++p))))))))))))))))))))))))))))
    {
        double cutZ = CS1.Z0 + key.H - std::min(p * mill.DeltaH, Zdept);
        snakePass(cuts1, CS1, +1, shift1, cutZ);
        snakePass(cuts2, CS2, -1, shift2, cutZ);
        betweenPass(cutZ);
    }
}

snakeCutting::snakeCut snakeCutting::getGlobalMaxCut(const QVector<snakeCut>& cuts1, const QVector<snakeCut>& cuts2)
{
    snakeCut maxCut = cuts1[0];

    for (const auto& c : cuts1)
    {
        maxCut.B = std::max(maxCut.B, c.B);
        maxCut.D = c.D;
        maxCut.L = c.L;
    }

    for (const auto& c : cuts2)
    {
        maxCut.B = std::max(maxCut.B, c.B);
        maxCut.D = c.D;
        maxCut.L = c.L;
    }

    return maxCut;
}

snakeCutting::snakeCut snakeCutting::getGlobalMinCut(const QVector<snakeCut>& cuts1, const QVector<snakeCut>& cuts2)
{
    snakeCut minCut = cuts1[0];

    for (const auto& c : cuts1)
    {
        if (c.B < minCut.B)
        {
            minCut.B = c.B;
            minCut.L = c.L;
            minCut.D = c.D;
        }
    }

    for (const auto& c : cuts2)
    {
        if (c.B < minCut.B)
        {
            minCut.B = c.B;
            minCut.L = c.L;
            minCut.D = c.D;
        }
    }

    return minCut;
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

    double   xLeft = CS1.X0 + getMaxL(cuts1) + mill.D / 2.0 + mill.D * 0.8;
    double   xRight = CS2.X0 - getMaxL(cuts2) - mill.D / 2.0 - mill.D * 0.8;

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

void snakeCutting::cutsFilling1()
{   // наполняем массив вырезами
    cuts1.clear();
    cuts1.append({5, 8, 4});
    cuts1.append({15, 5, 2});
    cuts1.append({21, 6, 2});
    cuts1.append({27.5, 5, 2});
    cuts1.append({35, 7, 3});
    // cuts1.clear();
    // cuts1.append({10, 5, 1});
    // cuts1.append({20, 7, 1});
    // cuts1.append({25, 5, 1});
    // cuts1.append({30, 6, 1});
}

void snakeCutting::cutsFilling2()
{   // наполняем массив вырезами
    cuts2.clear();
    cuts2.append({10, 5, 1});
    cuts2.append({20, 7, 1});
    cuts2.append({25, 5, 1});
    cuts2.append({30, 6, 1});
    // cuts2.clear();
    // cuts2.append({5, 8, 4});
    // cuts2.append({15, 5, 2});
    // cuts2.append({21, 6, 2});
    // cuts2.append({27.5, 5, 2});
    // cuts2.append({35, 7, 3});
}
