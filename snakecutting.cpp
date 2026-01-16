#include "snakecutting.h"


snakeCutting::snakeCutting()
{}

// Метод передвижения по координатам (заглушка)
// X ось x, Y ось y, Z ось z
void snakeCutting::moveTo(double X, double Y, double Z)
{

}

void snakeCutting::cutting(Mill mill, Key key)
{
    // наполняем массив вырезами
    cuts.append({15, 3, 2});
    cuts.append({25, 4, 3});
    cuts.append({35, 5, 2});

    // подъезжаем к ключу (пока хз с + и -)
    snakeCutting::moveTo(cordS.X0 + cuts.last().L + mill.D / 2,
                         cordS.Y0 + key.L + 2 * mill.D,
                         cordS.Z0 + key.H + 2);
    // опускаемся по z на высоту ключа - 1ый проход
    snakeCutting::moveTo(cordS.X0 + cuts.last().L + mill.D / 2,
                         cordS.Y0 + key.L + 2 * mill.D,
                         cordS.Z0 + key.H - deltaD);    // минус 0.2 мм

    // 1 проход если ширина змейки от 1 до 2 диаметров фрезы
    if ((mill.D <= Hzm) && (Hzm <= 2 * mill.D))
    {
        for (int i = Zdept / mill.h; i > 0; i--)
        {   // сделать доп цикл по элементам массива cut[i]
            // по Х тоже самое
            snakeCutting::moveTo(cordS.X0 + cuts.last().L + mill.D / 2,
                                                                          cordS.Y0 + cuts.last().B - cuts.last().D / 2
                                                                          - deltaD,
                                                                          cordS.Z0 + key.H - deltaD);
        }
    }
    else
    {   // несколько проходов если толщина больше двух диаметров фрезы и меньше ширины ключа - 2 мм (2 мм условно)
        if ((Hzm >= 2 * mill.D) && (Hzm < key.W - 2))
        {}
    }
}
