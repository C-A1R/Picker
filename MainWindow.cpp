#include "MainWindow.h"
#include "FileSystemWidget/FileSystemWidget.h"
#include "ProjectWidget/ProjectWidget.h"
#include "Settings.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QApplication>
#include <QStyleHints>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUi();
    initMenuBar();
    setMinimumSize(QSize(640, 480));
    const int w = Settings::instance()->value(SETTINGS_WIDTH).toInt();
    const int h = Settings::instance()->value(SETTINGS_HEIGHT).toInt();
    resize(qMax(w, minimumWidth()), qMax(h, minimumHeight()));

    if (const bool isMaximized = Settings::instance()->value(SETTINGS_MAXIMAZED).toBool();
        isMaximized)
    {
        this->setWindowState(Qt::WindowMaximized);
    }

    const bool fs_hidden = Settings::instance()->value(SETTINGS_FS_HIDDEN).toBool();
    if (fs_hidden)
        FSBrowserWidget->hide();

    connect(hide_btn, &QPushButton::pressed, this, &MainWindow::slot_hideFSBrowser);
    connect(splitter, &QSplitter::splitterMoved, this, &MainWindow::slot_saveSplitterSizes);
}

MainWindow::~MainWindow()
{
    Settings::instance()->setValue(SETTINGS_MAXIMAZED, isMaximized());
    if (!isMaximized())
    {
        Settings::instance()->setValue(SETTINGS_WIDTH, width());
        Settings::instance()->setValue(SETTINGS_HEIGHT, height());
    }
    Settings::instance()->setValue(SETTINGS_FS_HIDDEN, FSBrowserWidget->isHidden());
}

void MainWindow::initUi()
{
    auto centralWidget = new QWidget(this);

    splitter = new QSplitter(centralWidget);
    splitter->setOrientation(Qt::Horizontal);

    FSBrowserWidget = new FileSystemWidget(splitter);
    splitter->addWidget(FSBrowserWidget);

    auto rightWidget = new QWidget(splitter);
    {
        hide_btn = new QPushButton(rightWidget);
        hide_btn->setFlat(true);
        hide_btn->setFixedSize(QSize(5, 100));
        projectWidget = new ProjectWidget(rightWidget);
        auto rightWidget_hLay = new QHBoxLayout(rightWidget);
        rightWidget_hLay->addWidget(hide_btn);
        rightWidget_hLay->addWidget(projectWidget);
        rightWidget_hLay->setContentsMargins(0, 0, 0, 0);
    }
    splitter->addWidget(rightWidget);

    if (const QStringList split_sizes = Settings::instance()->value(SETTINGS_SPLIT_SIZES).toStringList();
        split_sizes.count() == splitter->count())
    {
        splitter->setSizes(QList<int>() << split_sizes.at(0).toInt() << split_sizes.at(1).toInt());
    }
    else
    {
        splitter->setSizes(QList<int>{INT_MAX, INT_MAX});
    }

    auto main_vLay = new QVBoxLayout();
    main_vLay->addWidget(splitter);
    centralWidget->setLayout(main_vLay);

    setCentralWidget(centralWidget);
}

void MainWindow::initMenuBar()
{
    QMenuBar *menuBar = this->menuBar();
    {
        const bool isDarkTheme = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
        QMenu *menu = menuBar->addMenu("Файл");
        {
            auto act = new QAction("Открыть проект", this);
            const QIcon icon = isDarkTheme ? QIcon(":/buildWidget/ico/open_dark.svg")
                                           : QIcon(":/buildWidget/ico/open.svg");
            act->setIcon(icon);
            connect(act, &QAction::triggered, projectWidget, &ProjectWidget::slot_changeProject);
            menu->addAction(act);
        }
        {
            auto act = new QAction("Сохранить проект", this);
            const QIcon icon = isDarkTheme ? QIcon(":/buildWidget/ico/save_dark.svg")
                                           : QIcon(":/buildWidget/ico/save.svg");
            act->setIcon(icon);
            connect(act, &QAction::triggered, projectWidget, &ProjectWidget::slot_saveProject);
            menu->addAction(act);
        }
    }
    {
        QMenu *menu = menuBar->addMenu("Вид");
        {
            auto act = new QAction("Показать/скрыть проводник", this);
            connect(act, &QAction::triggered, this, &MainWindow::slot_hideFSBrowser);
            menu->addAction(act);
        }
    }
}

void MainWindow::slot_hideFSBrowser()
{
    if (FSBrowserWidget->isHidden())
        FSBrowserWidget->show();
    else
        FSBrowserWidget->hide();
}

void MainWindow::slot_saveSplitterSizes()
{
    const QStringList split_sizes{QString::number(splitter->sizes().at(0)), QString::number(splitter->sizes().at(1))};
    Settings::instance()->setValue(SETTINGS_SPLIT_SIZES, split_sizes);
}
