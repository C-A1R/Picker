#include "MainWindow.h"
#include "FileSystemWidget/FileSystemWidget.h"
#include "BuildWidget/BuildWidget.h"
#include "Settings.h"

#include <QSplitter>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUi();
    setMinimumSize(QSize(640, 480));
    const int w = Settings::instance()->value(SETTINGS_WIDTH).toInt();
    const int h = Settings::instance()->value(SETTINGS_HEIGHT).toInt();
    resize(qMax(w, minimumWidth()), qMax(h, minimumHeight()));
}

MainWindow::~MainWindow()
{
    Settings::instance()->setValue(SETTINGS_WIDTH, width());
    Settings::instance()->setValue(SETTINGS_HEIGHT, height());
    const QStringList split_sizes{QString::number(splitter->sizes().at(0)), QString::number(splitter->sizes().at(1))};
    Settings::instance()->setValue(SETTINGS_SPLIT_SIZES, split_sizes);
}

void MainWindow::initUi()
{
    auto centralWidget = new QWidget(this);

    splitter = new QSplitter(centralWidget);
    splitter->setOrientation(Qt::Horizontal);

    auto fileSystemWidget = new FileSystemWidget(splitter);
    splitter->addWidget(fileSystemWidget);

    auto buildWidget = new BuildWidget(splitter);
    splitter->addWidget(buildWidget);

    if (const QStringList split_sizes = Settings::instance()->value(SETTINGS_SPLIT_SIZES).toStringList();
        split_sizes.count() == splitter->count())
    {
        splitter->setSizes(QList<int>() << split_sizes.at(0).toInt() << split_sizes.at(1).toInt());
    }
    else
    {
        splitter->setSizes(QList<int>({INT_MAX, INT_MAX}));
    }

    auto main_vLay = new QVBoxLayout();
    main_vLay->addWidget(splitter);
    centralWidget->setLayout(main_vLay);

    setCentralWidget(centralWidget);
}
