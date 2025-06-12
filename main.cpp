#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QStringLiteral("%1 %2").arg(a.applicationName()).arg(APP_VERSION));
    w.show();
    return a.exec();
}
