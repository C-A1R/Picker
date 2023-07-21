#include "MainWindow.h"

#include <QSplitter>
#include <QVBoxLayout>

#include "FileSystemWidget/FileSystemWidget.h"
#include "BuildWidget/BuildWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUi();
    resize(800, 600);
}

void MainWindow::initUi()
{
    auto centralWidget = new QWidget(this);

    auto splitter = new QSplitter(centralWidget);
    splitter->setOrientation(Qt::Horizontal);

    auto fileSystemWidget = new FileSystemWidget(splitter);
    splitter->addWidget(fileSystemWidget);

    auto buildWidget = new BuildWidget(splitter);
    splitter->addWidget(buildWidget);

    splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));

    auto main_vLay = new QVBoxLayout();
    main_vLay->addWidget(splitter);
    centralWidget->setLayout(main_vLay);

    setCentralWidget(centralWidget);
}
