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

private:
    Ui::MainWindow *ui;

    int editing_stock_row;
    QStringList editing_stock_info;

    void update_equips();
    void update_techs();
    void update_steps();
    void update_stocks();

};
#endif // MAINWINDOW_H
