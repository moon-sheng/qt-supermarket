#include "mainwindow.h"
#include <QApplication>
#include "dbhelper.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication a(argc, argv);
    DBHelper::initDatabase();
    MainWindow w;
    w.show();

    return a.exec();
}
