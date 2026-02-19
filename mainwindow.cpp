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
    K1.reset(new Key());
    K2.reset(new Key());
    M.reset(new Mill());
    SC.reset(new snakeCutting());
    CS1.reset(new coordSystem());
    CS2.reset(new coordSystem());

    viewer = new PathViewer(this);

    snakeCutting::setViewer(viewer);

    SC->cutsFilling1();
    SC->cutsFilling2();

    // CS2->X0 = -20;
    // 1 true если упор в торец 2 true если у ключа база слева
    // QTimer::singleShot(0,
    // this,
    // [this](){
    // SC->singleCutting(*M, *K1, SC->cuts1, false, true, *CS1);
    // });

    CS2->X0 = 23;
    // QTimer::singleShot(0,
    // this,
    // [this](){
    // SC->singleCutting(*M, *K2, SC->cuts1, false, true, *CS2);
    // });

    QTimer::singleShot(0,
    this,
    [this](){
        SC->doubleCutting(*M, SC->cuts1, SC->cuts2, *CS1, *CS2, *K1, 1);
    });

    QWidget*     central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    layout->addWidget(viewer);
    layout->setContentsMargins(0, 0, 0, 0);

    setCentralWidget(central);

    setWindowTitle("Snake Cutting Viewer");
    resize(1000, 700);
}
