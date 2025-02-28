#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_btn_update_plan_clicked();

    void on_btn_add_stock_clicked();

    void on_btn_edit_stock_clicked();

    void on_btn_save_stock_clicked();

    void stock_tech_changed(QString tech);

    void on_btn_del_stock_clicked();

    void on_btn_cancel_stock_clicked();

    void on_btn_edit_equip_clicked();

    void on_btn_add_equip_clicked();

    void on_btn_save_equip_clicked();

    void on_btn_cancel_edit_equip_clicked();

    void on_btn_edit_equip_color_clicked();

    void on_btn_del_equip_clicked();

    void on_btn_edit_step_clicked();

    void on_btn_add_step_clicked();

    void on_btn_save_step_clicked();

    void on_btn_del_step_clicked();

    void on_btn_cancel_step_clicked();

    void on_btn_edit_tech_clicked();

    void on_btn_add_tech_clicked();

    void on_btn_save_tech_clicked();

    void on_btn_cancel_tech_clicked();

    void on_btn_del_tech_clicked();

private:
    Ui::MainWindow *ui;

    int editing_stock_row;
    QStringList editing_stock_info;

    int editing_equip_row;
    QStringList editing_equip_info;

    int editing_step_row;
    QString editing_step_name;

    int editing_tech_row;
    QString editing_tech_name;
    QStringList editing_tech_info;

    void update_equips();
    void update_techs();
    void update_steps();
    void update_stocks();

};
#endif // MAINWINDOW_H
