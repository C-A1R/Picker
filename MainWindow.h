#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class FileExplorerWidget;
class ProjectWidget;
class QSplitter;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    FileExplorerWidget  *fileExplorerWidget{nullptr};
    ProjectWidget       *projectWidget{nullptr};
    QSplitter           *splitter{nullptr};
    QPushButton         *hideFileExplorer_btn{nullptr};

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
