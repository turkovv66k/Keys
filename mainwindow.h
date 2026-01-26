#pragma once

#include <QMainWindow>

class Key;
class Mill;
class snakeCutting;
class PathViewer;

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

    PathViewer*   viewer;
};
