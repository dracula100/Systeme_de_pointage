#include "splashscreen.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("/home/huzer/appli/BDD/db.sqlite");
    db.setDatabaseName("appli/BDD/db.sqlite");
    db.open();
    QApplication a(argc, argv);
    SplashScreen w;
    w.show();
    return a.exec();
}
