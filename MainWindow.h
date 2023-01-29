#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr);
private:
    void initUi();
};

#endif // MAINWINDOW_H
