#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sqlop.h"
#include "plan.h"

#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 设置日期控件显示值，默认为当前日期
    ui->date_beg->setDate(QDate::currentDate());
    ui->date_end->setDate(QDate::currentDate());

    // ui->stocks->setEditTriggers(QAbstractItemView::SelectedClicked);
    // ui->stocks->setEditTriggers(QAbstractItemView::NoEditTriggers);

    update_equips();
    update_techs();
    update_steps();

    // insertData();

    update_stocks();
    editing_stock_row = -1;
    // 隐藏取消按钮
    ui->btn_cancel_stock->setVisible(false);

    //checkTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 刷新equip表格
void MainWindow::update_equips()
{
    QVector<QStringList> equips = SqlOP::getInstance()->getEquipments();

    // 标题为 "设备编号","设备名称","稼动率","状态","最大负荷（时/半日）"
    ui->equips->setColumnCount(5);
    ui->equips->setHorizontalHeaderLabels(
        QStringList() << "设备编号" << "设备名称" << "稼动率" << "状态" << "最大负荷（时/半日）");
    
    // 关闭表格行头
    ui->equips->verticalHeader()->setVisible(false);

    // 清空表格
    ui->equips->setRowCount(0);

    // 填充表格
    // 最后一列（标记颜色）不显示，该行前两列背景色为标记颜色
    for (int i = 0; i < equips.size(); i++) {
        ui->equips->insertRow(i);
        for (int j = 0; j < 5; j++) {
            QTableWidgetItem *item = new QTableWidgetItem(equips[i][j]);
            ui->equips->setItem(i, j, item); 
            if (j < 2) {
                QColor color = QColor(equips[i][5]);
                item->setBackground(QBrush(color));
                
                double testGray = (0.299*color.red() + 0.587*color.green() + 0.114*color.blue()) / 255;
                color = testGray > 0.5 ? Qt::black : Qt::white;
                item->setForeground(QBrush(color));
            }
        }
    }
    // 设置表格列宽
    ui->equips->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void MainWindow::update_techs()
{
    QStringList techs = SqlOP::getInstance()->getTechs();

    // 标题为 "工艺名称" 和 "工序"
    ui->techs->setColumnCount(2);
    ui->techs->setHorizontalHeaderLabels(
        QStringList() << "工艺名称" << "工序");

    // 关闭表格行头
    ui->techs->verticalHeader()->setVisible(false);

    // 清空表格
    ui->techs->setRowCount(0);

    // 填充表格
    for (int i = 0; i < techs.size(); i++) {
        ui->techs->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem(techs[i]);
        ui->techs->setItem(i, 0, item); 

        // 在单元格右侧加入工序
        QStringList steps = SqlOP::getInstance()->getStepsOrderOfTech(techs[i]);
        QString step_str;
        for (int j = 0; j < steps.size(); j++) {
            step_str += steps[j] + " >> ";
        }
        step_str = step_str.left(step_str.size() - 3);
        item = new QTableWidgetItem(step_str);
        ui->techs->setItem(i, 1, item);
    } 

    // 设置表格列宽, 第一列自适应
    ui->techs->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->techs->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

}

void MainWindow::update_steps()
{
    QStringList steps = SqlOP::getInstance()->getSteps();

    // 标题为 "工序" 和 "设备"
    ui->steps->setColumnCount(2);
    ui->steps->setHorizontalHeaderLabels(
        QStringList() << "工序名称" << "设备");

    // 关闭表格行头
    ui->steps->verticalHeader()->setVisible(false);

    // 清空表格
    ui->steps->setRowCount(0);

    // 填充表格
    for (int i = 0; i < steps.size(); i++) {
        ui->steps->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem(steps[i]);
        ui->steps->setItem(i, 0, item);  

        // 在单元格右侧加入设备
        QStringList equips = SqlOP::getInstance()->getEquipmentsOfStep(steps[i]);
        QString equip_str;
        for (int j = 0; j < equips.size(); j++) {
            equip_str += equips[j] + "/";
        }
        equip_str = equip_str.left(equip_str.size() - 1);
        item = new QTableWidgetItem(equip_str);
        ui->steps->setItem(i, 1, item);
    }

    // 设置表格列宽, 第一列自适应
    ui->steps->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->steps->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}


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

}


void MainWindow::on_btn_update_plan_clicked()
{
    QDate beg = ui->date_beg->date();
    QDate end = ui->date_end->date();

    int diff = beg.daysTo(end);

    PlanManager *plan_manager = new PlanManager(beg, end);
    plan_manager->makePlan();
    QVector<QVector<Cell>> plans = plan_manager->getPlans();
    QVector<QStringList> stocks_infos = plan_manager->getStocksInfos();

    // 关闭表格行头、列头
    ui->plans->verticalHeader()->setVisible(false);
    // ui->plans->horizontalHeader()->setVisible(false);

    // 清空表格
    ui->plans->setRowCount(0);
    // 前两列为 存货编号,存货全名，合并单元格，其余列分三行，第一行为计划加工数量，
    // 第二行为计划时间，时间背景色为设备标记颜色，第三行为实际时间
    
    // 第一行为列头，内容为日期，从第三列开始
    ui->plans->setColumnCount(plans[0].size() + 3);
    
    //ui->plans->setItem(0, 0, new QTableWidgetItem(QString("存货编号")));
    //ui->plans->setItem(0, 1, new QTableWidgetItem(QString("工单号")));
    // 在第一行填入日期
    /*for (int i = 0; i < diff; i++) {
        QString date = beg.addDays(i).toString("MM-dd") + " 白班";
        QTableWidgetItem *item = new QTableWidgetItem(date);
        ui->plans->setItem(0, 2 * i + 3, item);
        date = beg.addDays(i).toString("MM-dd") + " 夜班";
        item = new QTableWidgetItem(date);
        ui->plans->setItem(0, 2 * i + 4, item);
    }*/

    // 设置列头
    ui->plans->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("存货编号")));
    ui->plans->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("工单号")));
    ui->plans->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("指标名称")));
    for (int i = 0; i < diff; i++) {
        QString date = beg.addDays(i).toString("MM-dd") + " 白班";
        QTableWidgetItem *item = new QTableWidgetItem(date);
        ui->plans->setHorizontalHeaderItem(2 * i + 3, item); 
        date = beg.addDays(i).toString("MM-dd") + " 夜班";
        item = new QTableWidgetItem(date);
        ui->plans->setHorizontalHeaderItem(2 * i + 4, item);
    }

    int last_row = -1;
    for (int i = 0; i < plans.size(); i++) {
        int max_row = 1;

        ui->plans->insertRow(last_row + 1); 
        ui->plans->insertRow(last_row + 2);
        ui->plans->insertRow(last_row + 3);
        QTableWidgetItem *item = new QTableWidgetItem(QString("计划数量"));
        ui->plans->setItem(last_row + 1, 2, item);
        item = new QTableWidgetItem(QString("计划时间"));
        ui->plans->setItem(last_row + 2, 2, item);
        item = new QTableWidgetItem(QString("实际时间"));
        ui->plans->setItem(last_row + 3, 2, item);

        for (int j = 0; j < plans[i].size(); j++) {
            int cell_row = plans[i][j].steps.size();
            if (cell_row > max_row) {
                for (int k = 0; k < cell_row - max_row; k++) {
                    ui->plans->insertRow(last_row + max_row * 3 + 1 + k * 3);
                    ui->plans->insertRow(last_row + max_row * 3 + 2 + k * 3);
                    ui->plans->insertRow(last_row + max_row * 3 + 3 + k * 3); 

                    item = new QTableWidgetItem(QString("计划数量"));
                    ui->plans->setItem(last_row + max_row * 3 + 1 + k * 3, 2, item);
                    item = new QTableWidgetItem(QString("计划时间"));
                    ui->plans->setItem(last_row + max_row * 3 + 2 + k * 3, 2, item);
                    item = new QTableWidgetItem(QString("实际时间"));
                    ui->plans->setItem(last_row + max_row * 3 + 3 + k * 3, 2, item);
                }
                max_row = cell_row;
            }
            for (int k = 0; k < cell_row; k++) {
                item = new QTableWidgetItem(QString::number(plans[i][j].nums[k]));
                ui->plans->setItem(last_row + 1 + k * 3, j + 3, item);
                item = new QTableWidgetItem(QString::number(plans[i][j].costs[k]));
                ui->plans->setItem(last_row + 2 + k * 3, j + 3, item);

                // 设置背景色
                QColor color = QColor(SqlOP::getInstance()->getColor(plans[i][j].equips[k]));
                item->setBackground(QBrush(color));
                double testGray = (0.299*color.red() + 0.587*color.green() + 0.114*color.blue()) / 255;
                color = testGray > 0.5? Qt::black : Qt::white;
                item->setForeground(QBrush(color));
            }
        }

        // 合并单元格
        ui->plans->setSpan(last_row + 1, 0, max_row * 3, 1);
        ui->plans->setSpan(last_row + 1, 1, max_row * 3, 1);
        // 填入存货编号和工单
        item = new QTableWidgetItem(stocks_infos[i][0]);
        ui->plans->setItem(last_row + 1, 0, item);
        item = new QTableWidgetItem(stocks_infos[i][1]);
        ui->plans->setItem(last_row + 1, 1, item);

        last_row += max_row * 3;
    }


    // 关闭表格行头、列头
    ui->loads->verticalHeader()->setVisible(false);
    // ui->loads->horizontalHeader()->setVisible(false);

    // 清空表格
    ui->loads->setRowCount(1);
    ui->loads->setColumnCount(plans[0].size() + 2);

    // 更新负荷
    QVector<QMap<QString, double>> loads = plan_manager->getLoads();
    // 在第一行填入日期
    /*for (int i = 0; i < diff; i++) {
        QString date = beg.addDays(i).toString("MM-dd") + " 白班";
        QTableWidgetItem *item = new QTableWidgetItem(date);
        ui->loads->setItem(0, 2 * i + 2, item);
        date = beg.addDays(i).toString("MM-dd") + " 夜班";
        item = new QTableWidgetItem(date);
        ui->loads->setItem(0, 2 * i + 3, item);
    }*/

    // 设置列头
    ui->loads->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("设备编号")));
    ui->loads->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("设备名称")));
    for (int i = 0; i < diff; i++) {
        QString date = beg.addDays(i).toString("MM-dd") + " 白班";
        QTableWidgetItem *item = new QTableWidgetItem(date);
        ui->loads->setHorizontalHeaderItem(2 * i + 2, item);
        date = beg.addDays(i).toString("MM-dd") + " 夜班";
        item = new QTableWidgetItem(date);
        ui->loads->setHorizontalHeaderItem(2 * i + 3, item);
    }
    
    QVector<QStringList> equips = SqlOP::getInstance()->getEquipments();
    for (int i = 0; i < equips.size(); i++) {
        // 设置背景色
        QColor color = QColor(equips[i][5]);
        double testGray = (0.299*color.red() + 0.587*color.green() + 0.114*color.blue()) / 255;
        QColor fcolor = testGray > 0.5? Qt::black : Qt::white;

        ui->loads->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem(equips[i][0]);
        ui->loads->setItem(i, 0, item);
        item->setBackground(QBrush(color));
        item->setForeground(QBrush(fcolor));
        item = new QTableWidgetItem(equips[i][1]);
        ui->loads->setItem(i, 1, item);
        item->setBackground(QBrush(color));
        item->setForeground(QBrush(fcolor));

        QString equip_id = equips[i][0];
        for (int j = 0; j < loads.size(); j++) {
            double l = loads[j][equip_id];
            item = new QTableWidgetItem(QString::number(l));
            ui->loads->setItem(i, j + 2, item);
            // 0 ~ MaxLoad-MAX_DIFF 为绿色， MaxLoad-MAX_DIFF ~ MaxLoad 为黄色， >MaxLoad 为红色       
            QColor color;
            double max_load = equips[i][4].toDouble();
            if (l < max_load - MIN_DIFF) {
                color = Qt::green;
            } else if (l < max_load) {
                color = Qt::yellow;
            } else {
                color = Qt::red;
            }
            item->setBackground(QBrush(color));
            double testGray = (0.299*color.red() + 0.587*color.green() + 0.114*color.blue()) / 255;
            color = testGray > 0.5? Qt::black : Qt::white;
            item->setForeground(QBrush(color));
        }
    }
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
    editing_stock_row = -1;
    editing_stock_info.clear();

    // 恢复表格
    update_stocks();

    // 恢复按钮状态
    ui->btn_add_stock->setEnabled(true);
    ui->btn_edit_stock->setEnabled(true);
    ui->btn_del_stock->setEnabled(true);

    // 隐藏取消按钮
    ui->btn_cancel_stock->setVisible(false);
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
    editing_stock_row = -1;
    editing_stock_info.clear();
    update_stocks();
    ui->btn_add_stock->setEnabled(true);
    ui->btn_edit_stock->setEnabled(true);
    ui->btn_del_stock->setEnabled(true);

    // 隐藏取消按钮
    ui->btn_cancel_stock->setVisible(false);
    return;
}

