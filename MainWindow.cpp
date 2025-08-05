#include "MainWindow.h"
#include "FileExplorerWidget/FileExplorerWidget.h"
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

    if (Settings::instance()->value(SETTINGS_FILE_EXP_HIDDEN).toBool())
        fileExplorerWidget->hide();

    connect(hideFileExplorer_btn, &QPushButton::pressed, this, &MainWindow::slot_hideFSBrowser);
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
    Settings::instance()->setValue(SETTINGS_FILE_EXP_HIDDEN, fileExplorerWidget->isHidden());
}

void MainWindow::initUi()
{
    auto centralWidget = new QWidget(this);

    hideFileExplorer_btn = new QPushButton(splitter);
    hideFileExplorer_btn->setFlat(true);
    hideFileExplorer_btn->setFixedSize(QSize(5, 100));

    splitter = new QSplitter(centralWidget);
    splitter->setOrientation(Qt::Horizontal);

    fileExplorerWidget = new FileExplorerWidget(splitter);
    splitter->addWidget(fileExplorerWidget);

    projectWidget = new ProjectWidget(splitter);
    splitter->addWidget(projectWidget);

    if (const QStringList split_sizes = Settings::instance()->value(SETTINGS_SPLIT_SIZES).toStringList();
        split_sizes.count() == splitter->count())
    {
        splitter->setSizes(QList<int>() << split_sizes.at(0).toInt() << split_sizes.at(1).toInt());
    }
    else
    {
        splitter->setSizes(QList<int>{INT_MAX, INT_MAX});
    }

    auto main_hLay = new QHBoxLayout();
    main_hLay->addWidget(hideFileExplorer_btn);
    main_hLay->addWidget(splitter);
    centralWidget->setLayout(main_hLay);

    setCentralWidget(centralWidget);
}

void MainWindow::initMenuBar()
{
    const bool isDarkTheme = QApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
    QMenuBar *menuBar = this->menuBar();
    {
        QMenu *menu = menuBar->addMenu("Файл");
        {
            auto act = ProjectWidget::createOpenAction(isDarkTheme, this);
            connect(act, &QAction::triggered, projectWidget, &ProjectWidget::slot_changeProject);
            menu->addAction(act);
        }
        {
            auto act = ProjectWidget::createSaveAction(isDarkTheme, this);
            connect(act, &QAction::triggered, projectWidget, &ProjectWidget::slot_saveProject);
            menu->addAction(act);
        }
    }
    {
        QMenu *menu = menuBar->addMenu("Правка");
        {
            auto act = ProjectWidget::createUndoAction(isDarkTheme, this);
            connect(act, &QAction::triggered, projectWidget, &ProjectWidget::slot_undo);
            connect(projectWidget, &ProjectWidget::signal_canUndoChanged, act, &QAction::setEnabled);
            menu->addAction(act);
        }
        {
            auto act = ProjectWidget::createRedoAction(isDarkTheme, this);
            connect(act, &QAction::triggered, projectWidget, &ProjectWidget::slot_redo);
            connect(projectWidget, &ProjectWidget::signal_canRedoChanged, act, &QAction::setEnabled);
            menu->addAction(act);
        }
    }
    {
        QMenu *menu = menuBar->addMenu("Вид");
        {
            auto act = new QAction("Проводник", this);
            const QIcon icon = isDarkTheme ? QIcon(":/project/ico/dock-left_dark.svg")
                                           : QIcon(":/project/ico/dock-left.svg");
            act->setIcon(icon);
            act->setCheckable(true);
            act->setChecked(!Settings::instance()->value(SETTINGS_FILE_EXP_HIDDEN).toBool());
            connect(act, &QAction::triggered, this, &MainWindow::slot_hideFSBrowser);
            connect(hideFileExplorer_btn, &QPushButton::clicked, this, [act, this](){ act->setChecked(!fileExplorerWidget->isHidden()); });
            menu->addAction(act);
        }
    }
    {
        QMenu *menu = menuBar->addMenu("Проект");
        {
            auto act = ProjectWidget::createBuildAction(isDarkTheme, this);
            connect(act, &QAction::triggered, projectWidget, &ProjectWidget::slot_build);
            menu->addAction(act);
        }
    }
}

void MainWindow::slot_hideFSBrowser()
{
    fileExplorerWidget->isHidden() ? fileExplorerWidget->show()
                                   : fileExplorerWidget->hide();
}

void MainWindow::slot_saveSplitterSizes()
{
    const QStringList split_sizes{QString::number(splitter->sizes().at(0)), QString::number(splitter->sizes().at(1))};
    Settings::instance()->setValue(SETTINGS_SPLIT_SIZES, split_sizes);
}
