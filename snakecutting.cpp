#include "PathViewer.h"
#include "snakecutting.h"
#include <algorithm>
#include <math.h>
#include <QDebug>


snakeCutting::snakeCutting():
    key(new Key()),
    mill(new Mill())
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
void snakeCutting::singleCutting( Mill& mill,
                                  Key& key,
                                  QVector<snakeCut>& cuts,
                                 bool isBaseSupport,
                                 bool isLeftSide,
                                 coordSystem& CS)
{
    if (isBaseSupport)
    {
        // заменяем L с кноца в начало если упор в торец и сбрасываем длину ключа key.L
        cuts = offsetCuts(cuts,key);
    }

    // коэффициент расчёта X координаты если от левой стороны то 1 если от правой то -1
    int k = 1;


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
                                CS.Z0 + key.H - std::min(i * mill.DeltaH, Zdept)); // минус 0.2 мм
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
        return cuts;

    int f = 0;
    while (f < cuts.length())
    {
        cuts[f].B -= key.L;
        ++f;
    }

    key.L = 0;

    return cuts;
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

void snakeCutting::middlePocketFill(Mill& mill, coordSystem& csLeft, coordSystem& csRight,bool isBaseSupport)
{
    if (cuts1.isEmpty() || cuts2.isEmpty()) {
        qDebug() << "middlePocketFill: cuts1 или cuts2 пустые";
        return;
    }
    if (isBaseSupport)
    {
        key->L =0;
    }

    // ───────────────────────────────────────────────────────────────
    // Находим самую нижнюю и самую верхнюю точки среди всех вырезов
    // ───────────────────────────────────────────────────────────────
    double minB = std::numeric_limits<double>::max();
    double maxB = std::numeric_limits<double>::lowest();

    for (const auto& cut : cuts1) {
        if (cut.B < minB) minB = cut.B;
        if (cut.B > maxB) maxB = cut.B;
    }
    for (const auto& cut : cuts2) {
        if (cut.B < minB) minB = cut.B;
        if (cut.B > maxB) maxB = cut.B;
    }

    // Защита от пустых/некорректных данных
    if (minB == std::numeric_limits<double>::max() || maxB == std::numeric_limits<double>::lowest()) {
        minB = 0.0;
        maxB = key->L;
    }

    // Можно добавить небольшой запас, если нужно перекрыть края
    // maxB += 1.0;   // например
    // minB -= 1.0;

    // ───────────────────────────────────────────────────────────────
    // Основные константы
    // ───────────────────────────────────────────────────────────────
    const double Zdepth   = key->H;
    const int    zPasses  = static_cast<int>(std::ceil(Zdepth / mill.DeltaH));
    const double stepY    = mill.D * 0.75;
    const double inset    = 0.15;
    const double centerX  = (csLeft.X0 + csRight.X0) / 2.0;
    const double safeY    = key->L + 2*mill.D;

    // ───────────────────────────────────────────────────────────────
    // Цикл по слоям глубины
    // ───────────────────────────────────────────────────────────────
    for (int p = 1; p <= zPasses; ++p)
    {
        double currentZ = csLeft.Z0 + key->H - std::min(static_cast<double>(p) * mill.DeltaH, Zdepth);

        moveTo(centerX, csLeft.Y0 + safeY, csLeft.Z0 + key->H + 4);
        moveTo(centerX, csLeft.Y0 + safeY, currentZ);

        int zigDirection = 1;

        // Теперь начинаем не с key->L, а с найденного maxB
        for (double y = maxB; y >= minB; y -= stepY)
        {
            double Lleft  = getL(cuts1, y);
            double Lright = getL(cuts2, y);

            double xWallLeft  = csLeft.X0  + Lleft  + mill.D / 2.0;
            double xWallRight = csRight.X0 - Lright - mill.D / 2.0;

            double pocketLeft  = xWallLeft  + inset;
            double pocketRight = xWallRight - inset;
            double pocketWidth = pocketRight - pocketLeft;

            if (pocketWidth <= 0.4) {
                continue;
            }

            if (pocketWidth <= 1.2) {
                double xCenter = (pocketLeft + pocketRight) / 2.0;
                moveTo(xCenter, csLeft.Y0 + y, currentZ);
                continue;
            }

            double mid    = (pocketLeft + pocketRight) / 2.0;
            double offset = pocketWidth * 0.25;

            if (zigDirection > 0) {
                moveTo(mid - offset, csLeft.Y0 + y, currentZ);
                moveTo(mid + offset, csLeft.Y0 + y, currentZ);
            } else {
                moveTo(mid + offset, csLeft.Y0 + y, currentZ);
                moveTo(mid - offset, csLeft.Y0 + y, currentZ);
            }

            zigDirection = -zigDirection;
        }

        moveTo(centerX, csLeft.Y0 + safeY, csLeft.Z0 + key->H + 4);
    }

    qDebug() << "middlePocketFill завершён, слоёв по Z:" << zPasses
             << ", Y от" << maxB << "до" << minB;
}

// в snakeCutting.cpp
double snakeCutting::getL(const QVector<snakeCut>& cuts, double y) const
{
    if (cuts.isEmpty()) {
        return 0.0;
    }

    // Создаём копию и сортируем по B (на случай, если cuts не отсортированы)
    QVector<snakeCut> sorted = cuts;
    std::sort(sorted.begin(), sorted.end(), [](const snakeCut& a, const snakeCut& b) {
        return a.B < b.B;
    });

    // Если y ниже самого нижнего выреза → берём самый нижний
    if (y <= sorted.first().B) {
        return sorted.first().L;
    }

    // Если y выше самого верхнего → берём самый верхний
    if (y >= sorted.last().B) {
        return sorted.last().L;
    }

    // Ищем интервал
    for (int i = 0; i < sorted.size() - 1; ++i) {
        double b1 = sorted[i].B;
        double b2 = sorted[i + 1].B;

        if (y >= b1 && y <= b2) {
            double t = (y - b1) / (b2 - b1);
            return sorted[i].L + t * (sorted[i + 1].L - sorted[i].L);
        }
    }

    // На всякий случай (не должно происходить)
    return sorted.last().L;
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

