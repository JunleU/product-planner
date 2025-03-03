#include "dialogselectstocks.h"
#include "ui_dialogselectstocks.h"
#include "checkboxheaderview.h"

#include "sqlop.h"

DialogSelectStocks::DialogSelectStocks(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DialogSelectStocks)
{
    ui->setupUi(this);
    updateStocks();
}

DialogSelectStocks::~DialogSelectStocks()
{
    delete ui;
}

void DialogSelectStocks::setPlanId(QString plan_id)
{
    this->plan_id = plan_id;
    QStringList dates = SqlOP::getInstance()->getDatesOfPlan(plan_id);
    if (dates.size() == 2) {
        beg = QDate::fromString(dates[0], "yyyy-MM-dd");
        end = QDate::fromString(dates[1], "yyyy-MM-dd");
    } else {
        beg = QDate::currentDate();
        // +30day
        end = beg.addDays(30);
    }

    ui->plan_id_edit->setText(plan_id);
    ui->beg->setDate(beg);
    ui->end->setDate(end);
    if (mode) {
        // 设置为禁用
        ui->plan_id_edit->setEnabled(false);
        ui->beg->setEnabled(false);
        ui->end->setEnabled(false);
    }
}

void DialogSelectStocks::updateStocks()
{
    // 获取所有存货信息
    QVector<QStringList> stocks = SqlOP::getInstance()->getStocks();
    // 清空表格
    ui->table_stocks->setRowCount(0);
    // 设置行数
    ui->table_stocks->setRowCount(stocks.size());
    // 设置列数
    ui->table_stocks->setColumnCount(stocks[0].size());
    // 设置表头
    QStringList header;
    header << "" << "存货编号" << "存货全名" << "规格" << "型号" << "工艺" << "工单号" << "计划数量" << "交货期限";
    ui->table_stocks->setHorizontalHeaderLabels(header);
    // 设置表格内容
    for (int i = 0; i < stocks.size(); i++) {
        for (int j = 0; j < stocks[i].size(); j++) {
            QTableWidgetItem *item = new QTableWidgetItem(stocks[i][j]);
            ui->table_stocks->setItem(i, j + 1, item);
        }
    }

    // 设置表格不可编辑
    ui->table_stocks->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置表头选择框
    CheckBoxHeaderView *cheader = new CheckBoxHeaderView(0, QPoint(3, 3), QSize(20, 20), Qt::Horizontal, this);
    ui->table_stocks->setHorizontalHeader(cheader);
    connect(cheader, &CheckBoxHeaderView::signalCheckStateChanged, [=](bool state){
        for (int i = 0; i < stocks.size(); i++) {
            QTableWidgetItem *item = ui->table_stocks->item(i, 0);
            item->setCheckState(state? Qt::Checked : Qt::Unchecked);
        }
    });
    
    // 设置表格选择框
    for (int i = 0; i < stocks.size(); i++) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setCheckState(Qt::Unchecked);
        ui->table_stocks->setItem(i, 0, item);
    }

    // 设置表格列宽，第一列根据内容自适应，其余列均分
    //ui->table_stocks->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    
    ui->table_stocks->resizeColumnsToContents();
    for (int i = 2; i < header.size()-5; i++) {
        ui->table_stocks->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        // ui->table_stocks->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
    //ui->table_stocks->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

void DialogSelectStocks::on_buttonBox_accepted()
{
    // 新建
    if (mode == 0) {
        QStringList dates;
        dates << beg.toString("yyyy-MM-dd") << end.toString("yyyy-MM-dd");
        plan_id = ui->plan_id_edit->text();
        if (plan_id.isEmpty()) {
            QMessageBox::information(nullptr, "提示", "请输入计划名称");
            return;
        }
        QVector<QStringList> stock_infos;
        for (int i = 0; i < ui->table_stocks->rowCount(); i++) {
            if (ui->table_stocks->item(i, 0)->checkState() == Qt::Checked) {
                QStringList l;
                l << ui->table_stocks->item(i, 1)->text() << ui->table_stocks->item(i, 6)->text();
                stock_infos.push_back(l);
            }
        }
        if (stock_infos.size() == 0) {
            QMessageBox::information(nullptr, "提示", "请选择存货");
            return;
        }
        if (SqlOP::getInstance()->createPlan(plan_id, dates, stock_infos)) {
            QMessageBox::information(nullptr, "提示", "创建成功");
            accept();
        } else {
            QMessageBox::information(nullptr, "提示", "创建失败");
        }
    } else {
        // 修改
        QVector<QStringList> stock_infos;
        for (int i = 0; i < ui->table_stocks->rowCount(); i++) {
            if (ui->table_stocks->item(i, 0)->checkState() == Qt::Checked) {
                QStringList l;
                l << ui->table_stocks->item(i, 1)->text() << ui->table_stocks->item(i, 6)->text();
                stock_infos.push_back(l);
            }
        }
        if (stock_infos.size() == 0) {
            QMessageBox::information(nullptr, "提示", "请选择存货");
            return;
        }
        if (SqlOP::getInstance()->updatePlanStocks(plan_id, stock_infos)) {
            QMessageBox::information(nullptr, "提示", "修改成功");
            accept();
        } else {
            QMessageBox::information(nullptr, "提示", "修改失败");
        }
    }
}


void DialogSelectStocks::on_buttonBox_rejected()
{
    reject();
}

