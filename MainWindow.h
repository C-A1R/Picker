#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QSplitter;

class MainWindow : public QMainWindow
{
    QSplitter *splitter {nullptr};
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void initUi();
};

#endif // MAINWINDOW_H
