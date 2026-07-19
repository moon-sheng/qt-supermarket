#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "goodsmanagewidget.h"
#include "membermanagewidget.h"
#include "statswidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}


//商品管理
    void MainWindow::on_actionGoods_triggered()
    {
        GoodsManageWidget *w = new GoodsManageWidget(this);
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->show();
    }
    //会员管理
        void MainWindow::on_actionMember_triggered()
        {
            MemberManageWidget *w = new MemberManageWidget(this);
            w->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动释放内存
            w->show();
        }
//收银结算

        void MainWindow::on_actionCheckout_triggered()
        {
            CheckoutWidget *w = new CheckoutWidget(this);
            w->setAttribute(Qt::WA_DeleteOnClose);
            w->show();
        }
//统计看板
        void MainWindow::on_actionStats_triggered()
        {
            StatsWidget *w = new StatsWidget(this);
            w->setAttribute(Qt::WA_DeleteOnClose);
            w->show();
        }
