#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sqlop.h"
#include "plan.h"

#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>


void MainWindow::update_stocks()
{
    QVector<QStringList> stocks = SqlOP::getInstance()->getStocks();

    // 标题为 存货编号,存货全名,规格,型号,工艺,工艺参数,工单号,计划数量,交货期限
    ui->stocks->setColumnCount(9);
    ui->stocks->setHorizontalHeaderLabels(
        QStringList() << "存货编号" << "存货全名" << "规格" << "型号" << 
        "工艺" << "工艺参数" << "工单号" << "计划数量" << "交货期限"); 
    
    

    // 关闭表格行头
    // ui->stocks->verticalHeader()->setVisible(false);

    // 清空表格
    ui->stocks->setRowCount(0);

    // 填充表格
    // int last_row = 0;
    for (int i = 0; i < stocks.size(); i++) {
        // 每一项所占的行数取决于工艺参数的行数，即工序个数，除工艺参数外，其余单元格均合并
        QStringList steps = SqlOP::getInstance()->getStepsOfTech(stocks[i][4]);
        // int row_count = steps.size();
        ui->stocks->insertRow(i);
        for (int j = 0; j < 8; j++) {
            // 合并单元格
            // ui->stocks->setSpan(last_row, j > 4 ? j + 1 : j, row_count, 1);
            QTableWidgetItem *item = new QTableWidgetItem(stocks[i][j]);
            // 工艺参数先空着
            ui->stocks->setItem(i, j > 4 ? j + 1 : j, item);

            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        } 
        // 填充工艺参数(格式为 工序  参数\n)
        QString param;
        for (int j = 0; j < steps.size(); j++) {
            param += steps[j] + "  " +
                SqlOP::getInstance()->getTechParam(stocks[i][0], stocks[i][5], steps[j]) + "\n";
        }
        param = param.left(param.size() - 1);
        QTableWidgetItem *item = new QTableWidgetItem(param);
        ui->stocks->setItem(i, 5, item);

        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    }

    // 设置表格列宽
    ui->stocks->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    // 设置表格行高
    ui->stocks->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    // 初始化编辑状态
    editing_stock_row = -1;
    editing_stock_info.clear();

    
    ui->btn_add_stock->setEnabled(true);
    ui->btn_edit_stock->setEnabled(true);
    ui->btn_del_stock->setEnabled(true);

    // 隐藏取消按钮
    ui->btn_cancel_stock->setVisible(false);
}

void MainWindow::on_btn_add_stock_clicked()
{
    // 添加一行
    ui->stocks->insertRow(ui->stocks->rowCount());
    // 初始化单元格
    for (int i = 0; i <= 8; i++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        ui->stocks->setItem(ui->stocks->rowCount() - 1, i, item);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    }
    // 将当前行设置为新行
    ui->stocks->setCurrentCell(ui->stocks->rowCount() - 1, 0);
    on_btn_edit_stock_clicked();
}


void MainWindow::on_btn_edit_stock_clicked()
{
    // 获取选中的行
    int row = ui->stocks->currentRow();
    if (row == -1) {
        QMessageBox::information(this, "提示", "请选择要编辑的行");
        return;
    }
    // 保存状态
    editing_stock_row = row;
    editing_stock_info.clear();
    for (int i = 0; i <= 8; i++) {
        editing_stock_info.append(ui->stocks->item(row, i)->text());
    }

    // 禁用添加、编辑、和删除按钮
    ui->btn_add_stock->setEnabled(false);
    ui->btn_edit_stock->setEnabled(false);
    ui->btn_del_stock->setEnabled(false);

    // 显示取消按钮
    ui->btn_cancel_stock->setVisible(true);

    // 前4列设置可编辑
    for (int i = 0; i < 4; i++) {
        QTableWidgetItem *item = ui->stocks->item(row, i);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    }
    // 第7列设置可编辑
    QTableWidgetItem *item = ui->stocks->item(row, 6);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);

    int num = 0;
    if (editing_stock_info[7] != "") {
        num = editing_stock_info[7].toInt(); 
    }
    // 第8列设置为spinbox
    QSpinBox *spinbox = new QSpinBox();
    spinbox->setMinimum(0);
    spinbox->setMaximum(1000000);
    ui->stocks->setCellWidget(row, 7, spinbox);

    // 初始化spinbox的值
    spinbox->setValue(num);

    // 获取第5列的值
    QString tech = ui->stocks->item(row, 4)->text();
    
    // 第5列改为下拉框
    QComboBox *combo = new QComboBox();
    combo->addItems(SqlOP::getInstance()->getTechs());
    ui->stocks->setCellWidget(row, 4, combo);

    // 初始化下拉框文本
    combo->setCurrentText(tech);
    tech = combo->currentText();

    // comb设置为不可编辑
    combo->setEditable(false);

    // 第六列根据第五列的选择自动更新
    // 第五列的选择改变时，第六列更新SpinBox的个数，有几个工序就有几个SpinBox
    connect(combo, SIGNAL(currentTextChanged(QString)), this, SLOT(stock_tech_changed(QString)));

    QString param_str = ui->stocks->item(row, 5)->text();
    
    QStringList steps = SqlOP::getInstance()->getStepsOfTech(tech);
    // 第七列设置为SpinBox
    QGridLayout *layout = new QGridLayout();
    for (int i = 0; i < steps.size(); i++) {
        QLabel *label = new QLabel(steps[i]);
        QSpinBox *spinbox = new QSpinBox();
        spinbox->setMinimum(0); 
        spinbox->setMaximum(10000);
        layout->addWidget(label, i, 0);
        layout->addWidget(spinbox, i, 1);
    }
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    // 初始化spinbox的值
    if (param_str != "") {
        QStringList params = param_str.split("\n");
        for (int i = 0; i < params.size(); i++) {
            QStringList param = params[i].split("  ");
            QString step = param[0];
            int num = param[1].toInt(); 
            for (int j = 0; j < steps.size(); j++) {
                if (step == steps[j]) {
                    QSpinBox *spinbox = (QSpinBox*)layout->itemAtPosition(j, 1)->widget();
                    spinbox->setValue(num);
                    break;
                } 
            }
        }
    }
    
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    ui->stocks->setCellWidget(row, 5, widget);


    // 第九列设置为日期控件，可用日历
    QString date_str = ui->stocks->item(row, 8)->text();
    QDateEdit *date = new QDateEdit();
    date->setDate(QDate::currentDate());
    ui->stocks->setCellWidget(row, 8, date);
    date->setCalendarPopup(true);
    // 初始化日期控件
    if (date_str != "") {
        date->setDate(QDate::fromString(date_str, "yyyy-MM-dd"));
    }

    // 调整表格高度
    ui->stocks->resizeRowsToContents();
}

void MainWindow::stock_tech_changed(QString tech)
{
    // 获取工序
    QStringList steps = SqlOP::getInstance()->getStepsOfTech(tech);

    // 删除第六列的控件
    ui->stocks->removeCellWidget(ui->stocks->currentRow(), 5);

    // 添加新的控件
    QGridLayout *layout = new QGridLayout();

    // 按以下形式排列
    // 工序1 SpinBox1
    // 工序2 SpinBox2
    // ...
    for (int i = 0; i < steps.size(); i++) {
        QLabel *label = new QLabel(steps[i]);
        QSpinBox *spinbox = new QSpinBox();
        spinbox->setMinimum(0); 
        spinbox->setMaximum(10000);
        layout->addWidget(label, i, 0);
        layout->addWidget(spinbox, i, 1);
    }

    // 把layout中的间隙设置为0
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);


    // 添加到表格中
    QWidget *widget2 = new QWidget();
    widget2->setLayout(layout);
    ui->stocks->setCellWidget(ui->stocks->currentRow(), 5, widget2);

    // 调整表格高度
    ui->stocks->resizeRowsToContents();
}


void MainWindow::on_btn_save_stock_clicked()
{
    if (editing_stock_row == -1) {
       return; 
    }

    QString stock_id = editing_stock_info[0];
    QString work_order = editing_stock_info[6];

    // 前四列直接读
    QStringList stock_info;
    for (int i = 0; i < 4; i++) {
        stock_info.append(ui->stocks->item(editing_stock_row, i)->text());
    }

    // 第五列读下拉框
    QComboBox *combo = (QComboBox*)ui->stocks->cellWidget(editing_stock_row, 4);
    stock_info.append(combo->currentText());

    QVector<int> param;
    // 第六列读SpinBox
    QWidget *widget = ui->stocks->cellWidget(editing_stock_row, 5);
    QGridLayout *layout = (QGridLayout*)widget->layout();
    for (int i = 0; i < layout->count() / 2; i++) {
        QSpinBox *spinbox = (QSpinBox*)layout->itemAtPosition(i,1)->widget();
        param.append(spinbox->value());
    }
    // 读7列
    stock_info.append(ui->stocks->item(editing_stock_row, 6)->text());

    // 读8列
    QSpinBox *spinbox = (QSpinBox*)ui->stocks->cellWidget(editing_stock_row, 7);
    stock_info.append(QString::number(spinbox->value()));

    // 读9列
    QDateEdit *date = (QDateEdit*)ui->stocks->cellWidget(editing_stock_row, 8);
    stock_info.append(date->date().toString("yyyy-MM-dd"));

    if (!SqlOP::getInstance()->updateStock(stock_id, work_order, stock_info, param))
        return;

    // 恢复状态
    //editing_stock_row = -1;
    //editing_stock_info.clear();

    // 恢复表格
    update_stocks();
}


void MainWindow::on_btn_del_stock_clicked()
{
    // 获取选中的行
    int row = ui->stocks->currentRow();
    if (row == -1) {
        QMessageBox::information(this, "提示", "请选择要删除的行");
        return;
    }

    // 再次确认
    if (QMessageBox::question(this, "提示", "是否确认删除该行？") == QMessageBox::No)
        return;

    // 获取存货编号和工单号
    QString stock_id = ui->stocks->item(row, 0)->text();
    QString work_order = ui->stocks->item(row, 6)->text();

    // 删除
    if (!SqlOP::getInstance()->deleteStock(stock_id, work_order))
        return;

    update_stocks();
}


void MainWindow::on_btn_cancel_stock_clicked()
{
    // 恢复状态
    // editing_stock_row = -1;
    // editing_stock_info.clear();
    update_stocks();
    return;
}

