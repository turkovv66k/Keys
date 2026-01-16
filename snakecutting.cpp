#include "snakecutting.h"


snakeCutting::snakeCutting()
{}

// Метод передвижения по координатам (заглушка)
// X ось x, Y ось y, Z ось z
void snakeCutting::moveTo(double X, double Y, double Z)
{

}

void snakeCutting::cutting(Mill M, Key K)
{
    // 1 проход если ширина змейки от 1 до 2 диаметров фрезы
    if ((M.D <= Hzm) && (Hzm <= 2 * M.D))
    {
        dept = K.Hkey - Hzm;

        for (int i = dept / M.h; i > 0; i--)
        {
            snakeCutting::moveTo( , , );
        }
    }
    else
    {   // несколько проходов если толщина больше двух диаметров фрезы и меньше ширины ключа - 2 мм (2 мм условно)
        if ((Hzm >= 2 * M.D) && (Hzm < K.Wkey - 2))
        {}
    }
}
