#pragma once

#include <QMainWindow>

class Key;
class Mill;
class snakeCutting;
class PathViewer;
class coordSystem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Key*          K;
    Mill*         M;
    snakeCutting* SC;
    coordSystem*  CS1;
    coordSystem*  CS2;

    PathViewer*   viewer;
};
