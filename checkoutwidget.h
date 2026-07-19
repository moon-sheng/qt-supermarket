#ifndef CHECKOUTWIDGET_H
#define CHECKOUTWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSqlQueryModel>
#include <QSqlQuery>

namespace Ui {
class CheckoutWidget;
}

class CheckoutWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CheckoutWidget(QWidget *parent = nullptr);
    ~CheckoutWidget();

private slots:
    void on_btnSearchProduct_clicked();
    void on_btnAddToCart_clicked();
    void on_btnApplyMember_clicked();
    void on_btnCheckout_clicked();
    void on_btnClearCart_clicked();
    void on_tableViewCart_doubleClicked(const QModelIndex &index); // 可选：双击修改数量

private:
    Ui::CheckoutWidget *ui;
    QStandardItemModel *cartModel;          // 购物车数据模型
    QSqlQueryModel *searchModel;            // 搜索商品结果模型（临时）
    double discount;                        // 当前会员折扣（默认为1.0，即无折扣）
    int currentMemberId;                    // 当前会员ID（如果有）
    void updateTotalLabels();               // 更新总价标签
    bool addToCart(int productId, int quantity); // 添加商品到购物车（内部）
    void clearCart();                       // 清空购物车
    bool checkout();                        // 执行结算
};

#endif // CHECKOUTWIDGET_H
