#include "mainwindow.h"

#include "Key.h"
#include "Mill.h"
#include "PathViewer.h"
#include "snakeCutting.h"

#include <QPushButton>
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


    SC->cutting(*M, *K, true, true);

    QWidget*     central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    QPushButton* button = new QPushButton;

    button->setText("hello");

    layout->addWidget(viewer);
    layout->addWidget(button);
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
