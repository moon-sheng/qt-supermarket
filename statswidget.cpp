#include "statswidget.h"
#include "ui_statswidget.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

StatsWidget::StatsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatsWidget)
{
    ui->setupUi(this);
    // 设置表格列头
    ui->tableRanking->setColumnCount(3);
    ui->tableRanking->setHorizontalHeaderLabels(QStringList() << "商品名" << "总销量" << "总销售额");
    ui->tableRanking->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    loadStats(); // 加载初始数据
}

StatsWidget::~StatsWidget()
{
    delete ui;
}

void StatsWidget::on_btnRefresh_clicked()
{
    loadStats();
}

void StatsWidget::loadStats()
{
    QSqlQuery query;

    // 1. 总销售额（所有销售记录的总额）
    double totalSales = 0;
    if (query.exec("SELECT SUM(total_price) FROM sales")) {
        if (query.next()) {
            totalSales = query.value(0).toDouble();
        }
    } else {
        qDebug() << "查询总销售额失败:" << query.lastError().text();
    }

    // 2. 总订单数（按销售记录条数计算，或者按时间分组合并，这里简单按记录数算）
    int orderCount = 0;
    if (query.exec("SELECT COUNT(*) FROM sales")) {
        if (query.next()) {
            orderCount = query.value(0).toInt();
        }
    }

    // 3. 总销售商品件数
    int totalQuantity = 0;
    if (query.exec("SELECT SUM(quantity) FROM sales")) {
        if (query.next()) {
            totalQuantity = query.value(0).toInt();
        }
    }

    // 更新标签
    ui->labelTotalSales->setText(QString("总销售额：￥%1").arg(totalSales, 0, 'f', 2));
    ui->labelOrderCount->setText(QString("总订单数：%1").arg(orderCount));
    ui->labelTotalQuantity->setText(QString("总销量：%1 件").arg(totalQuantity));

    // 4. 商品销售排行（按销量降序）
    QString sql = "SELECT p.name, SUM(s.quantity) AS total_qty, SUM(s.total_price) AS total_sales "
                  "FROM sales s JOIN products p ON s.product_id = p.id "
                  "GROUP BY s.product_id "
                  "ORDER BY total_qty DESC";
    query.exec(sql);

    ui->tableRanking->setRowCount(0);
    int row = 0;
    while (query.next()) {
        QString name = query.value(0).toString();
        int qty = query.value(1).toInt();
        double sales = query.value(2).toDouble();
        ui->tableRanking->insertRow(row);
        ui->tableRanking->setItem(row, 0, new QTableWidgetItem(name));
        ui->tableRanking->setItem(row, 1, new QTableWidgetItem(QString::number(qty)));
        ui->tableRanking->setItem(row, 2, new QTableWidgetItem(QString::number(sales, 'f', 2)));
        row++;
    }
}
