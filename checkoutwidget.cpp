#include "checkoutwidget.h"
#include "ui_checkoutwidget.h"
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

CheckoutWidget::CheckoutWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckoutWidget)
{
    ui->setupUi(this);

    // 初始化购物车模型（5列：商品ID、名称、单价、数量、小计）
    cartModel = new QStandardItemModel(0, 5, this);
    cartModel->setHorizontalHeaderLabels(QStringList() << "ID" << "商品名" << "单价" << "数量" << "小计");
    ui->tableViewCart->setModel(cartModel);
    ui->tableViewCart->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 隐藏ID列（第0列）
    ui->tableViewCart->setColumnHidden(0, true);

    // 初始化搜索结果的临时模型（用于下拉框显示）
    searchModel = new QSqlQueryModel(this);

    // 默认无折扣
    discount = 1.0;
    currentMemberId = -1;
    ui->labelMemberInfo->setText("未使用会员");
    updateTotalLabels();

    // 设置数量微调框的范围
    ui->spinQuantity->setRange(1, 9999);
    ui->spinQuantity->setValue(1);
}

CheckoutWidget::~CheckoutWidget()
{
    delete ui;
}

// ---------- 搜索商品 ----------
void CheckoutWidget::on_btnSearchProduct_clicked()
{
    QString keyword = ui->lineEdit_search->text().trimmed();
    if (keyword.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入商品编号或名称进行搜索");
        return;
    }

    // 查询商品（按编号精确或名称模糊）
    QString sql = "SELECT id, code, name, price, stock FROM products WHERE code = ? OR name LIKE ?";
    searchModel->setQuery(sql);
    searchModel->setQuery(sql);
    // 注意：setQuery 无法直接绑定参数，我们用 QSqlQuery 预处理，但为了简单，直接拼接（注意防止注入，但这里示范）
    // 为了更安全，使用 QSqlQuery 然后设值给 searchModel
    // 但 QSqlQueryModel 不直接支持参数，我们可以用 QSqlQuery 然后 setQuery 带参数，但更保险：
    QSqlQuery query;
    query.prepare(sql);
    query.addBindValue(keyword);
    query.addBindValue("%" + keyword + "%");
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "查询失败：" + query.lastError().text());
        return;
    }

    // 将查询结果设置到 searchModel（但 QSqlQueryModel 不能直接接受绑定的 query，需用 setQuery 不带参数）
    // 因此我们换一种更简单的方法：直接执行 query，然后构造一个列表填充到 comboBox
    // 方法：清空 comboBox，遍历 query 添加项目
    ui->comboProduct->clear();
    while (query.next()) {
        int id = query.value(0).toInt();
        QString code = query.value(1).toString();
        QString name = query.value(2).toString();

        int stock = query.value(4).toInt();
        // 显示格式：编号-名称 (库存:xx)
        QString display = QString("%1 - %2 (库存:%3)").arg(code).arg(name).arg(stock);
        ui->comboProduct->addItem(display, id); // 存储商品ID
    }

    if (ui->comboProduct->count() == 0) {
        QMessageBox::information(this, "提示", "未找到匹配的商品");
    } else {
        ui->comboProduct->setCurrentIndex(0);
    }
}

// ---------- 添加到购物车 ----------
void CheckoutWidget::on_btnAddToCart_clicked()
{
    int currentIndex = ui->comboProduct->currentIndex();
    if (currentIndex < 0) {
        QMessageBox::warning(this, "提示", "请先搜索并选择商品");
        return;
    }

    int productId = ui->comboProduct->currentData().toInt();
    int quantity = ui->spinQuantity->value();

    // 查询当前商品库存
    QSqlQuery query;
    query.prepare("SELECT stock FROM products WHERE id = ?");
    query.addBindValue(productId);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "错误", "无法获取商品库存");
        return;
    }
    int stock = query.value(0).toInt();
    if (quantity > stock) {
        QMessageBox::warning(this, "库存不足", QString("当前库存仅 %1 件").arg(stock));
        return;
    }

    // 检查购物车是否已有该商品，如果有则累加数量（但需考虑库存上限）
    bool found = false;
    for (int row = 0; row < cartModel->rowCount(); ++row) {
        int existingId = cartModel->item(row, 0)->text().toInt();
        if (existingId == productId) {
            int oldQty = cartModel->item(row, 3)->text().toInt();
            int newQty = oldQty + quantity;
            if (newQty > stock) {
                QMessageBox::warning(this, "库存不足", QString("当前库存仅 %1 件，购物车已有 %2 件").arg(stock).arg(oldQty));
                return;
            }
            // 更新数量和小计
            cartModel->item(row, 3)->setText(QString::number(newQty));
            double price = cartModel->item(row, 2)->text().toDouble();
            double subtotal = price * newQty;
            cartModel->item(row, 4)->setText(QString::number(subtotal, 'f', 2));
            found = true;
            break;
        }
    }

    if (!found) {
        // 查询商品详细信息
        QSqlQuery query2;
        query2.prepare("SELECT name, price FROM products WHERE id = ?");
        query2.addBindValue(productId);
        if (!query2.exec() || !query2.next()) {
            QMessageBox::warning(this, "错误", "无法获取商品信息");
            return;
        }
        QString name = query2.value(0).toString();
        double price = query2.value(1).toDouble();

        // 新增一行
        int row = cartModel->rowCount();
        cartModel->insertRow(row);
        cartModel->setItem(row, 0, new QStandardItem(QString::number(productId)));
        cartModel->setItem(row, 1, new QStandardItem(name));
        cartModel->setItem(row, 2, new QStandardItem(QString::number(price, 'f', 2)));
        cartModel->setItem(row, 3, new QStandardItem(QString::number(quantity)));
        double subtotal = price * quantity;
        cartModel->setItem(row, 4, new QStandardItem(QString::number(subtotal, 'f', 2)));
    }

    // 更新总价显示
    updateTotalLabels();
    QMessageBox::information(this, "成功", "已添加到购物车");
}

// ---------- 应用会员折扣 ----------
void CheckoutWidget::on_btnApplyMember_clicked()
{
    QString input = ui->lineEditMemberID->text().trimmed();
    if (input.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入会员ID或手机号");
        return;
    }

    // 判断输入是否为数字（ID）或字符串（手机号）
    bool isNumber;
    int id = input.toInt(&isNumber);
    QSqlQuery query;
    if (isNumber) {
        query.prepare("SELECT id, name, discount FROM members WHERE id = ?");
        query.addBindValue(id);
    } else {
        query.prepare("SELECT id, name, discount FROM members WHERE phone = ?");
        query.addBindValue(input);
    }

    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "提示", "未找到该会员，请检查输入");
        ui->labelMemberInfo->setText("未使用会员");
        discount = 1.0;
        currentMemberId = -1;
        updateTotalLabels();
        return;
    }

    currentMemberId = query.value(0).toInt();
    QString name = query.value(1).toString();
    discount = query.value(2).toDouble();
    ui->labelMemberInfo->setText(QString("会员：%1 (折扣：%2)").arg(name).arg(discount));
    updateTotalLabels();
    QMessageBox::information(this, "成功", "会员折扣已应用");
}

// ---------- 更新总价标签 ----------
void CheckoutWidget::updateTotalLabels()
{
    double totalBefore = 0.0;
    for (int row = 0; row < cartModel->rowCount(); ++row) {
        double subtotal = cartModel->item(row, 4)->text().toDouble();
        totalBefore += subtotal;
    }
    double totalAfter = totalBefore * discount;
    ui->labelTotalBefore->setText(QString("折前总价：￥%1").arg(totalBefore, 0, 'f', 2));
    ui->labelTotalAfter->setText(QString("折后总价：￥%1").arg(totalAfter, 0, 'f', 2));
}

// ---------- 清空购物车 ----------
void CheckoutWidget::on_btnClearCart_clicked()
{
    clearCart();
}

void CheckoutWidget::clearCart()
{
    cartModel->removeRows(0, cartModel->rowCount());
    discount = 1.0;
    currentMemberId = -1;
    ui->labelMemberInfo->setText("未使用会员");
    ui->lineEditMemberID->clear();
    updateTotalLabels();
}

// ---------- 结算 ----------
void CheckoutWidget::on_btnCheckout_clicked()
{
    if (cartModel->rowCount() == 0) {
        QMessageBox::warning(this, "提示", "购物车为空，请先添加商品");
        return;
    }

    // 确认结算
    if (QMessageBox::question(this, "确认结算", "确定要结算当前购物车吗？") != QMessageBox::Yes)
        return;

    // 执行结算
    if (checkout()) {
        QMessageBox::information(this, "成功", "结算成功！销售记录已保存。");
        clearCart(); // 清空购物车
    } else {
        QMessageBox::warning(this, "错误", "结算失败，请查看日志");
    }
}

bool CheckoutWidget::checkout()
{
    QSqlDatabase::database().transaction(); // 开启事务

    QDateTime now = QDateTime::currentDateTime();
    QString timeStr = now.toString("yyyy-MM-dd hh:mm:ss");

    // 遍历购物车
    for (int row = 0; row < cartModel->rowCount(); ++row) {
        int productId = cartModel->item(row, 0)->text().toInt();
        int quantity = cartModel->item(row, 3)->text().toInt();
        double price = cartModel->item(row, 2)->text().toDouble();
        double subtotal = price * quantity; // 折前小计
        double finalTotal = subtotal * discount; // 折后小计

        // 插入销售记录
        QSqlQuery queryInsert;
        queryInsert.prepare("INSERT INTO sales (product_id, quantity, total_price, sale_time) VALUES (?, ?, ?, ?)");
        queryInsert.addBindValue(productId);
        queryInsert.addBindValue(quantity);
        queryInsert.addBindValue(finalTotal);
        queryInsert.addBindValue(timeStr);
        if (!queryInsert.exec()) {
            qDebug() << "插入销售记录失败:" << queryInsert.lastError().text();
            QSqlDatabase::database().rollback();
            return false;
        }

        // 更新库存（减少）
        QSqlQuery queryUpdate;
        queryUpdate.prepare("UPDATE products SET stock = stock - ? WHERE id = ?");
        queryUpdate.addBindValue(quantity);
        queryUpdate.addBindValue(productId);
        if (!queryUpdate.exec()) {
            qDebug() << "更新库存失败:" << queryUpdate.lastError().text();
            QSqlDatabase::database().rollback();
            return false;
        }
    }

    // 提交事务
    if (!QSqlDatabase::database().commit()) {
        qDebug() << "事务提交失败";
        return false;
    }
    return true;
}

// 可选：双击购物车修改数量（我们暂不实现，可后续扩展）
void CheckoutWidget::on_tableViewCart_doubleClicked(const QModelIndex &index)
{
    // 可以弹窗修改数量
}
