#include "mainwindow.h"



MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    K = new Key();
    M = new Mill();
    SC = new snakeCutting();
}

MainWindow::~MainWindow()
{
    SC->cutting(*M, *K);
}
