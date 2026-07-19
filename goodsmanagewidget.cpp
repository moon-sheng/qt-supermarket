#include "goodsmanagewidget.h"
#include "ui_goodsmanagewidget.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlError>

GoodsManageWidget::GoodsManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GoodsManageWidget)
{
    ui->setupUi(this);

    // 创建数据模型并绑定商品表
    model = new QSqlTableModel(this);
    model->setTable("products");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

GoodsManageWidget::~GoodsManageWidget()
{
    delete ui;
}

// 刷新（浏览所有）
void GoodsManageWidget::on_btnRefresh_clicked()
{
    model->setFilter("");
    model->select();
}

// 条件搜索（按名称）
void GoodsManageWidget::on_btnSearch_clicked()
{
    QString name = ui->lineEdit_search->text();
    if (name.isEmpty()) {
        model->setFilter("");
    } else {
        model->setFilter("name like '%" + name + "%'");
    }
    model->select();
}

// 新增商品
void GoodsManageWidget::on_btnAdd_clicked()
{
    QString code = QInputDialog::getText(this, "新增", "请输入商品编号:");
    if (code.isEmpty()) return;
    QString name = QInputDialog::getText(this, "新增", "请输入商品名称:");
    if (name.isEmpty()) return;
    double price = QInputDialog::getDouble(this, "新增", "请输入单价:");
    int stock = QInputDialog::getInt(this, "新增", "请输入库存数量:");

    QSqlQuery query;
    query.prepare("INSERT INTO products (code, name, price, stock) VALUES (?, ?, ?, ?)");
    query.addBindValue(code);
    query.addBindValue(name);
    query.addBindValue(price);
    query.addBindValue(stock);

    if (query.exec()) {
        QMessageBox::information(this, "成功", "商品添加成功！");
        model->select(); // 刷新表格
    } else {
        QMessageBox::warning(this, "失败", "添加失败：" + query.lastError().databaseText());
    }
}

// 按ID删除（条件删除）
void GoodsManageWidget::on_btnDelete_clicked()
{
    QString idStr = ui->lineEdit_delID->text();
    if (idStr.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入要删除的商品ID");
        return;
    }

    int id = idStr.toInt();
    QSqlQuery query;
    query.prepare("DELETE FROM products WHERE id = ?");
    query.addBindValue(id);

    if (query.exec()) {
        QMessageBox::information(this, "成功", "删除成功！");
        model->select();
        ui->lineEdit_delID->clear();
    } else {
        QMessageBox::warning(this, "失败", "删除失败：" + query.lastError().text());
    }
}

// 提交修改（双击表格修改后，点此保存）
void GoodsManageWidget::on_btnSubmit_clicked()
{
    if (model->submitAll()) {
        QMessageBox::information(this, "成功", "修改已保存！");
        model->select();
    } else {
        QMessageBox::warning(this, "失败", "保存失败：" + model->lastError().text());
    }
}
