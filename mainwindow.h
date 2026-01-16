#pragma once

#include "Key.h"
#include "mill.h"
#include "snakecutting.h"
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Key*          K = nullptr;
    Mill*         M = nullptr;
    snakeCutting* SC = nullptr;
};
