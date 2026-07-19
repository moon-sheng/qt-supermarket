#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QMessageBox>
#include <QMainWindow>
#include "membermanagewidget.h"
#include "checkoutwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void on_actionGoods_triggered();
    void on_actionMember_triggered();
    void on_actionCheckout_triggered();
    void on_actionStats_triggered();
};

#endif // MAINWINDOW_H
