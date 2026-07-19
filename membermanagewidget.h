#ifndef MEMBERMANAGEWIDGET_H
#define MEMBERMANAGEWIDGET_H

#include <QWidget>
#include <QSqlTableModel>

namespace Ui {
class MemberManageWidget;
}

class MemberManageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MemberManageWidget(QWidget *parent = nullptr);
    ~MemberManageWidget();

private slots:
    void on_btnRefresh_clicked();
    void on_btnSearch_clicked();
    void on_btnAdd_clicked();
    void on_btnDelete_clicked();
    void on_btnSubmit_clicked();

private:
    Ui::MemberManageWidget *ui;
    QSqlTableModel *model;
};

#endif // MEMBERMANAGEWIDGET_H
