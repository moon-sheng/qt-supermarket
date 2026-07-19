#include "membermanagewidget.h"
#include "ui_membermanagewidget.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <QInputDialog>
#include <QSqlError>
#include <QHeaderView>

MemberManageWidget::MemberManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MemberManageWidget)
{
    ui->setupUi(this);

    // 1. 创建数据模型并绑定 members 表
    model = new QSqlTableModel(this);
    model->setTable("members");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); // 手动提交
    model->select(); // 加载数据

    // 2. 将模型设置到表格视图
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MemberManageWidget::~MemberManageWidget()
{
    delete ui;
}

// ---------- 刷新（浏览所有） ----------
void MemberManageWidget::on_btnRefresh_clicked()
{
    model->setFilter(""); // 清除筛选
    model->select();
}

// ---------- 条件搜索（按姓名） ----------
void MemberManageWidget::on_btnSearch_clicked()
{
    QString name = ui->lineEdit_search->text();
    if (name.isEmpty()) {
        model->setFilter("");
    } else {
        // 参照商品管理的写法，按 name 模糊查询
        model->setFilter("name like '%" + name + "%'");
    }
    model->select();
}

// ---------- 新增会员 ----------
void MemberManageWidget::on_btnAdd_clicked()
{
    // 依次弹出对话框获取信息
    QString phone = QInputDialog::getText(this, "新增会员", "请输入手机号:");
    if (phone.isEmpty()) return;

    QString name = QInputDialog::getText(this, "新增会员", "请输入姓名:");
    if (name.isEmpty()) return;

    // 折扣：范围 0.1 ~ 1.0，默认 0.9，小数点后保留2位
    double discount = QInputDialog::getDouble(this, "新增会员", "请输入折扣(0.1~1.0):", 0.9, 0.1, 1.0, 2);

    QSqlQuery query;
    query.prepare("INSERT INTO members (phone, name, discount) VALUES (?, ?, ?)");
    query.addBindValue(phone);
    query.addBindValue(name);
    query.addBindValue(discount);

    if (query.exec()) {
        QMessageBox::information(this, "成功", "会员添加成功！");
        model->select(); // 刷新表格
    } else {
        QMessageBox::warning(this, "失败", "添加失败：" + query.lastError().databaseText());
    }
}

// ---------- 按ID删除会员 ----------
void MemberManageWidget::on_btnDelete_clicked()
{
    QString idStr = ui->lineEdit_delID->text();
    if (idStr.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入要删除的会员ID");
        return;
    }

    int id = idStr.toInt();
    QSqlQuery query;
    query.prepare("DELETE FROM members WHERE id = ?");
    query.addBindValue(id);

    if (query.exec()) {
        QMessageBox::information(this, "成功", "删除成功！");
        model->select();       // 刷新表格
        ui->lineEdit_delID->clear(); // 清空输入框
    } else {
        QMessageBox::warning(this, "失败", "删除失败：" + query.lastError().text());
    }
}

// ---------- 提交修改（双击表格修改后，点此保存） ----------
void MemberManageWidget::on_btnSubmit_clicked()
{
    if (model->submitAll()) {
        QMessageBox::information(this, "成功", "修改已保存！");
        model->select(); // 刷新显示最新数据
    } else {
        QMessageBox::warning(this, "失败", "保存失败：" + model->lastError().text());
    }
}
