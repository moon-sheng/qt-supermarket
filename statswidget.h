#ifndef STATSWIDGET_H
#define STATSWIDGET_H

#include <QWidget>

namespace Ui {
class StatsWidget;
}

class StatsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatsWidget(QWidget *parent = nullptr);
    ~StatsWidget();

private slots:
    void on_btnRefresh_clicked();

private:
    Ui::StatsWidget *ui;
    void loadStats();
};

#endif // STATSWIDGET_H
