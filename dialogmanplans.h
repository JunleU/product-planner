#ifndef DIALOGMANPLANS_H
#define DIALOGMANPLANS_H

#include <QDialog>

namespace Ui {
class DialogManPlans;
}

class DialogManPlans : public QDialog
{
    Q_OBJECT

public:
    explicit DialogManPlans(QWidget *parent = nullptr);
    ~DialogManPlans();

private slots:
    void on_btn_create_clicked();

    void on_btn_edit_clicked();

    void on_btn_del_clicked();

    void on_btn_quit_clicked();

private:
    void updatePlans();
    Ui::DialogManPlans *ui;
};

#endif // DIALOGMANPLANS_H
