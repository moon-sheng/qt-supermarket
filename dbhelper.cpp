#include "dbhelper.h"

void DBHelper::initDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("supermarket.db");

    if (!db.open()) {
        qDebug() << "数据库打开失败:" << db.lastError().text();
        return;
    }

    QSqlQuery query;

    // 创建商品表
    query.exec("CREATE TABLE IF NOT EXISTS products ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "code TEXT UNIQUE, "
               "name TEXT, "
               "price REAL, "
               "stock INTEGER)");

    // 创建会员表
    query.exec("CREATE TABLE IF NOT EXISTS members ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "phone TEXT UNIQUE, "
               "name TEXT, "
               "discount REAL)");

    // 创建销售记录表
    query.exec("CREATE TABLE IF NOT EXISTS sales ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "product_id INTEGER, "
               "quantity INTEGER, "
               "total_price REAL, "
               "sale_time DATETIME DEFAULT CURRENT_TIMESTAMP)");

    // 插入测试数据（如果不存在的话）
    query.exec("INSERT OR IGNORE INTO products (code, name, price, stock) VALUES "
               "('1001', '苹果', 5.5, 100), "
               "('1002', '香蕉', 3.0, 80), "
               "('1003', '可乐', 2.5, 200)");

    query.exec("INSERT OR IGNORE INTO members (phone, name, discount) VALUES "
               "('13800001111', '张三', 0.9), "
               "('13800002222', '李四', 0.85)");

    qDebug() << "数据库初始化成功！";
}
