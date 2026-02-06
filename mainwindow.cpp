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
    CS1 = new coordSystem();
    CS2 = new coordSystem();

    viewer = new PathViewer(this);

    snakeCutting::setViewer(viewer);

    // CS2->X0 = -20;
    QTimer::singleShot(0,
                          this,
                          [this](){
                              SC->cutting(*M, *K, false, true, *CS1);               // false       // 1 true если упор в
                                                                                    // торец
                          });                               // 2 true если у ключа база слева

    CS2->X0 = 20;
    QTimer::singleShot(0,
                       this,
                       [this](){
                           SC->cutting(*M, *K, false, false, *CS2);
                       });

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
    delete CS1;
    delete CS2;
}
