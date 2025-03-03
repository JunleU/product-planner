#ifndef DIALOGSELECTSTOCKS_H
#define DIALOGSELECTSTOCKS_H

#include <QDialog>
#include <QDate>

namespace Ui {
class DialogSelectStocks;
}

class DialogSelectStocks : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectStocks(QWidget *parent = nullptr);
    ~DialogSelectStocks();

    void setPlanId(QString plan_id);

    // 0-新建计划 1-修改计划
    void setMode(int mode) { this->mode = mode; }

    void updateStocks();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::DialogSelectStocks *ui;

    QString plan_id;
    QDate beg, end;

    int mode;
};

#endif // DIALOGSELECTSTOCKS_H
