#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QDateEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class PlanManager;
class Cell;

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

    void on_comb_plan_id_currentTextChanged(const QString &arg1);

    void on_btn_creat_plan_clicked();

    void on_btn_man_plan_clicked();

    void on_btn_output_clicked();

    void on_plan_show_menu(const QPoint &pos);

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

    QStringList added_stock_info; // 用于保存新增计划行的存货信息

    void update_equips();
    void update_techs();
    void update_steps();
    void update_stocks();

    void update_plans();
    void update_loads();

    void update_plan_info(QString init_plan_id);

    QVector<int> editing_plan_pos; // row, col
    // QStringList editing_plan_info;

    void on_plans_cell_clicked(int row, int col, QVector<int> old_info, double old_cost, QString equip_id);
    void plain_plan_cell_clicked(int row, int col);

    PlanManager *plan_manager;

    QVector<QVector<Cell>> plans_data;

    // QVector<int> begin_row; // 每个存货的开始行号

    int output_xlsx(QString plan_id);

};
#endif // MAINWINDOW_H
