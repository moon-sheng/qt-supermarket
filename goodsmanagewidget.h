#ifndef GOODSMANAGEWIDGET_H
#define GOODSMANAGEWIDGET_H

#include <QWidget>
#include <QSqlTableModel>

namespace Ui {
class GoodsManageWidget;
}

class GoodsManageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GoodsManageWidget(QWidget *parent = nullptr);
    ~GoodsManageWidget();

private slots:
    void on_btnRefresh_clicked();
    void on_btnSearch_clicked();
    void on_btnAdd_clicked();
    void on_btnDelete_clicked();
    void on_btnSubmit_clicked();

private:
    Ui::GoodsManageWidget *ui;
    QSqlTableModel *model;
};

#endif // GOODSMANAGEWIDGET_H
