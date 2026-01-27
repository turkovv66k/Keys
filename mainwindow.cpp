#include "mainwindow.h"

#include "Key.h"
#include "Mill.h"
#include "PathViewer.h"
#include "snakeCutting.h"

#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    K = new Key();
    M = new Mill();
    SC = new snakeCutting();

    viewer = new PathViewer(this);

    snakeCutting::setViewer(viewer);


    QTimer::singleShot(0,
                          this,
                          [this](){
                              SC->cutting(*M, *K, true, true);      // 1 true если упор в торец
                          });                               // 2 true если у ключа база слева




    QWidget*     central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    layout->addWidget(viewer);
    layout->setContentsMargins(0, 0, 0, 0);

    setCentralWidget(central);

    setWindowTitle("Snake Cutting Viewer");
    resize(1000, 700);
}

MainWindow::~MainWindow()
{
    delete K;
    delete M;
    delete SC;
}
