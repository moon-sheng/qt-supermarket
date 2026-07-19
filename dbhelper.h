#ifndef DBHELPER_H
#define DBHELPER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

class DBHelper
{
public:
    static void initDatabase();
};

#endif // DBHELPER_H
