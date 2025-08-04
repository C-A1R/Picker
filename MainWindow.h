#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class FileSystemWidget;
class ProjectWidget;
class QSplitter;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    FileSystemWidget    *FSBrowserWidget{nullptr};
    ProjectWidget       *projectWidget{nullptr};
    QSplitter           *splitter{nullptr};
    QPushButton         *hide_btn{nullptr};

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initUi();
    void initMenuBar();

private slots:
    void slot_hideFSBrowser();
    void slot_saveSplitterSizes();
};

#endif // MAINWINDOW_H
