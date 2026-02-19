#include "PathViewer.h"
#include "snakecutting.h"
#include <algorithm>
#include <math.h>
#include <QDebug>

snakeCutting::snakeCutting()
    : key(new Key())
    , mill(new Mill())
{
    qDebug() << "Конструктор snakeCutting вызван";
}

PathViewer* snakeCutting::pathViewer = nullptr;

void snakeCutting::moveTo(double X, double Y, double Z)
{
    qDebug() << "moveTo:" << X << Y << Z;
    if (pathViewer)
        pathViewer->addPoint(X, Y, Z);
}

void snakeCutting::setViewer(PathViewer* viewer)
{
    pathViewer = viewer;
}

void snakeCutting::singleCutting(Mill& mill, Key& key, QVector<snakeCut>& cuts,
                                 bool isBaseSupport, bool isLeftSide, coordSystem& CS)
{
    if (isBaseSupport)
    {
        cuts = offsetCuts(cuts, key);
        key.L = 0;
    }
    auto   maxCut = getFirstCut(cuts);
    double maxL   = maxCut.L;
    int    k      = getCutsSide(isLeftSide);

    moveTo(CS.X0 + k * (maxL + mill.D / 2), CS.Y0 + key.L + 2 * mill.D, CS.Z0 + key.H + 2);

    int passes = (int)ceil(Zdept / mill.DeltaH);

    for (int i = 1; i <= passes; i++)
    {
        double cutZ = CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept);
        moveTo(CS.X0 + k * (maxL + mill.D / 2), CS.Y0 + key.L + 2 * mill.D, cutZ);

        for (int j = cuts.length() - 1; j >= 0; j--)
        {
            double X  = CS.X0 + k * (cuts[j].L + mill.D / 2);
            double Y1 = CS.Y0 + cuts[j].B + cuts[j].D / 2.0 + deltaD;
            double Y2 = CS.Y0 + cuts[j].B - cuts[j].D / 2.0 - deltaD;

            if (j > 0)
            {
                moveTo(X, Y1, cutZ);
                moveTo(X, Y2, cutZ);
            }
            if (j == 0)
            {
                double Ybot = CS.Y0 + cuts[j].B - cuts[j].D / 2.0 + deltaD * 5;
                moveTo(X, Y1,   cutZ);
                moveTo(X, Ybot, cutZ);
                moveTo(X, Ybot, CS.Z0 + key.H + 2);
            }
        }
        moveTo(CS.X0 + k * (maxL + mill.D / 2), CS.Y0 + key.L + 2 * mill.D, CS.Z0 + key.H + 2);
    }
}

void snakeCutting::doubleCutting(Mill& mill, QVector<snakeCut>& cuts1, QVector<snakeCut>& cuts2,
                                 coordSystem& CS1, coordSystem& CS2, Key& key,
                                 double Zdept, bool isBaseSupport)
{
    if (isBaseSupport)
    {
        cuts1 = offsetCuts(cuts1, key);
        cuts2 = offsetCuts(cuts2, key);
            key.L = 0;
    }

    int    passesZ = (int)ceil(Zdept / mill.DeltaH);
    double stepB   = mill.D * 0.8;

    auto betweenPass = [&](double cutZ)
    {
        snakeCut minCut  = getGlobalMinCut(cuts1, cuts2);
        double   B_start = key.L;
        double   B_end   = minCut.B - deltaD * 5 - minCut.D / 2;
        bool     goRight = true;

        auto doRow = [&](double B)
        {
            double Y       = CS1.Y0 + B;
            double X_left  = CS1.X0 + (interpolateL(cuts1, B) + mill.D / 2.0) + stepB;
            double X_right = CS2.X0 - (interpolateL(cuts2, B) + mill.D / 2.0) - stepB;
            if (X_left >= X_right)
                return;
            if (goRight) { moveTo(X_left, Y, cutZ); moveTo(X_right, Y, cutZ); }
            else         { moveTo(X_right, Y, cutZ); moveTo(X_left, Y, cutZ); }
            goRight = !goRight;
        };

        double B = B_start;
        while (B > B_end)
        {
            doRow(B);
            B -= stepB;
            if (B < B_end) B = B_end;
        }
        doRow(B_end);
    };

    snakeCut firstCut1 = getFirstCut(cuts1);
    snakeCut firstCut2 = getFirstCut(cuts2);
    double   Zsafe     = CS1.Z0 + key.H + 2.0;

    moveTo(CS1.X0 + firstCut1.L + mill.D / 2, CS1.Y0 + key.L + 2 * mill.D, Zsafe);

    for (int p = 1; p <= passesZ; ++p)
    {
        double cutZ = CS1.Z0 + key.H - std::min(p * mill.DeltaH, Zdept);

        // 1. левая змейка сверху вниз
        moveTo(CS1.X0 + firstCut1.L + mill.D / 2, CS1.Y0 + key.L + 2 * mill.D, cutZ);
        for (int j = cuts1.length() - 1; j >= 0; j--)
        {
            double X  = CS1.X0 + cuts1[j].L + mill.D / 2;
            double Y1 = CS1.Y0 + cuts1[j].B + cuts1[j].D / 2.0 + deltaD;
            double Y2 = CS1.Y0 + cuts1[j].B - cuts1[j].D / 2.0 - deltaD;
            moveTo(X, Y1, cutZ);
            moveTo(X, Y2, cutZ);
            if (j == 0)
                moveTo(X, CS1.Y0 + cuts1[j].B - cuts1[j].D / 2.0 - deltaD * 5, cutZ);
        }

        // 2. переезд к правой змейке
        snakeCut minCut = getGlobalMinCut(cuts1, cuts2);
        double   Ytrans = CS1.Y0 + minCut.B - deltaD * 5 - minCut.D / 2.0;
        moveTo(CS2.X0 - firstCut2.L - mill.D / 2, Ytrans, cutZ);

        // 3. правая змейка снизу вверх
        for (int j = 0; j < cuts2.length(); j++)
        {
            double X  = CS2.X0 - cuts2[j].L - mill.D / 2;
            double Y2 = CS2.Y0 + cuts2[j].B - cuts2[j].D / 2.0 - deltaD;
            double Y1 = CS2.Y0 + cuts2[j].B + cuts2[j].D / 2.0 + deltaD;
            moveTo(X, Y2, cutZ);
            moveTo(X, Y1, cutZ);
            if (j == cuts2.length() - 1)
            {
                moveTo(X, CS2.Y0 + key.L + 2 * mill.D, cutZ);
                moveTo(X, CS2.Y0 + key.L + 2 * mill.D, Zsafe);
            }
        }

        // 4. зачистка
        betweenPass(cutZ);
    }
}

QVector<snakeCutting::snakeCut> snakeCutting::offsetCuts(QVector<snakeCut>& cuts, Key& key)
{
    if (cuts.isEmpty()) return cuts;
    for (auto& c : cuts) c.B -= key.L;
    return cuts;
}

snakeCutting::snakeCut snakeCutting::getFirstCut(QVector<snakeCut>& cuts)
{
    snakeCut maxCut = cuts.first();
    for (const auto& c : qAsConst(cuts))
        if (c.B > maxCut.B) { maxCut.B = c.B; maxCut.L = c.L; maxCut.D = c.D; }
    return maxCut;
}

snakeCutting::snakeCut snakeCutting::getLastCut(QVector<snakeCut>& cuts)
{
    snakeCut minCut = cuts.first();
    for (const auto& c : qAsConst(cuts))
        if (c.B < minCut.B) { minCut.B = c.B; minCut.L = c.L; minCut.D = c.D; }
    return minCut;
}

snakeCutting::snakeCut snakeCutting::getGlobalMinCut(const QVector<snakeCut>& cuts1,
                                                     const QVector<snakeCut>& cuts2)
{
    snakeCut minCut = cuts1[0];
    for (const auto& c : cuts1)
        if (c.B < minCut.B) { minCut = c; }
    for (const auto& c : cuts2)
        if (c.B < minCut.B) { minCut = c; }
    return minCut;
}

double snakeCutting::interpolateL(QVector<snakeCut>& cuts, double B)
{
    if (cuts.isEmpty()) return 0;
    if (B <= cuts.first().B) return cuts.first().L;
    if (B >= cuts.last().B)  return cuts.last().L;
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

double snakeCutting::getMaxL(const QVector<snakeCut>& cuts)
{
    double maxL = cuts[0].L;
    for (const auto& c : cuts) maxL = std::max(maxL, c.L);
    return maxL;
}

int snakeCutting::getCutsSide(bool isLeftSide)
{
    return isLeftSide ? 1 : -1;
}

void snakeCutting::cutsFilling1()
{
    cuts1.clear();
    cuts1.append({5,    8, 2.5});
    cuts1.append({15,   5, 2});
    cuts1.append({21,   6, 2});
    cuts1.append({27.5, 5, 2});
    cuts1.append({35,   7, 3});
}

void snakeCutting::cutsFilling2()
{
    cuts2.clear();
    cuts2.append({10, 5, 3});
    cuts2.append({20, 7, 1});
    cuts2.append({25, 5, 1});
    cuts2.append({30, 6, 3});
}
