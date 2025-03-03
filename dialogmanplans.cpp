#include "dialogmanplans.h"
#include "ui_dialogmanplans.h"
#include "dialogselectstocks.h"
#include "sqlop.h"

#include <QMessageBox>

DialogManPlans::DialogManPlans(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogManPlans)
{
    ui->setupUi(this);
    updatePlans();
}

DialogManPlans::~DialogManPlans()
{
    delete ui;
}


void DialogManPlans::on_btn_create_clicked()
{
    DialogSelectStocks *dialog = new DialogSelectStocks(this);
    dialog->setMode(0);
    dialog->setPlanId("");
    // 如果accept，更新table_palns
    if (dialog->exec() == QDialog::Accepted) {
        updatePlans();
    }
}


void DialogManPlans::on_btn_edit_clicked()
{
    int row = ui->table_plans->currentRow();
    if (row == -1) {
        QMessageBox::information(nullptr, "提示", "请选择一行");
        return;
    }
    QString plan_id = ui->table_plans->item(row, 0)->text();
    DialogSelectStocks *dialog = new DialogSelectStocks(this);
    dialog->setMode(1);
    dialog->setPlanId(plan_id);
    // 如果accept，更新table_palns
    if (dialog->exec() == QDialog::Accepted) {
        updatePlans();
    }
}


void DialogManPlans::on_btn_del_clicked()
{
    int row = ui->table_plans->currentRow();
    if (row == -1) {
        QMessageBox::information(nullptr, "提示", "请选择一行");
        return;
    }
    QString plan_id = ui->table_plans->item(row, 0)->text();
    if (QMessageBox::question(nullptr, "提示", "确定删除该计划吗？") == QMessageBox::Yes) {
        SqlOP::getInstance()->deletePlan(plan_id);
        updatePlans();
    }
}


void DialogManPlans::on_btn_quit_clicked()
{
    this->close();
}

void DialogManPlans::updatePlans()
{
    // 表头：计划编号，起始时间，结束时间
    QStringList plan_ids = SqlOP::getInstance()->getPlanIds();
    ui->table_plans->setRowCount(plan_ids.size());
    ui->table_plans->setColumnCount(3);
    ui->table_plans->setHorizontalHeaderLabels(QStringList() << "计划编号" << "起始时间" << "结束时间");
    for (int i = 0; i < plan_ids.size(); i++) {
        ui->table_plans->setItem(i, 0, new QTableWidgetItem(plan_ids[i]));
        QStringList dates = SqlOP::getInstance()->getDatesOfPlan(plan_ids[i]);
        ui->table_plans->setItem(i, 1, new QTableWidgetItem(dates[0]));
        ui->table_plans->setItem(i, 2, new QTableWidgetItem(dates[1]));
    }
    // 表格不可编辑
    ui->table_plans->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // 伸展
    ui->table_plans->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}
