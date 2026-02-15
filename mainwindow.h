#pragma once

#include <QMainWindow>
#include "Key.h"
#include "Mill.h"
#include "coordSystem.h"
#include "snakeCutting.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);


private:
   QScopedPointer<Key>          K1;
    QScopedPointer<Key>         K2;
   QScopedPointer<Mill>         M;

   QScopedPointer<snakeCutting> SC;
   QScopedPointer<coordSystem> CS1;
   QScopedPointer<coordSystem> CS2;

    PathViewer*   viewer;
};
