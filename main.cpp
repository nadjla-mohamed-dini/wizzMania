#include "mainwindow.h"

#include <QApplication>

//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//   MainWindow w;
//    w.show();
//    return a.exec();
//}

#include <QApplication>
#include <QtTest>
#include "usermanagertest.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Lancer les tests
    UserManagerTest test;
    return QTest::qExec(&test);
}
