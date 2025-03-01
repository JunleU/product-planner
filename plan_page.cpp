#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sqlop.h"
#include "plan.h"

#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>


void MainWindow::update_plans()
{
    // plan_manager->updatePlan();

    QDate beg = plan_manager->getBeg();
    QDate end = plan_manager->getEnd();
    int diff = beg.daysTo(end);

    plans_data = plan_manager->getPlans(); // TODO: 分离
    QVector<QStringList> stocks_infos = plan_manager->getStocksInfos();

    // 关闭表格行头、列头
    ui->plans->verticalHeader()->setVisible(false);
    // ui->plans->horizontalHeader()->setVisible(false);

    // 清空表格
    ui->plans->setRowCount(0);
    // 前两列为 存货编号,存货全名，合并单元格，其余列分三行，第一行为计划加工数量，
    // 第二行为计划时间，时间背景色为设备标记颜色，第三行为实际时间
    
    // 第一行为列头，内容为日期，从第三列开始
    ui->plans->setColumnCount(plans_data[0].size() + 3);

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
    for (int i = 0; i < plans_data.size(); i++) {
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

        for (int j = 0; j < plans_data[i].size(); j++) {
            int cell_row = plans_data[i][j].steps_id.size();
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
                // 左边为文本按钮，右边为CheckBox
                QHBoxLayout *layout = new QHBoxLayout(this);
                QPushButton *btn1 = new QPushButton(this);
                btn1->setText(QString::number(plans_data[i][j].nums[k]));
                layout->addWidget(btn1);
                // 只显示文本，不显示背景色，不显示边框，不显示焦点
                btn1->setFlat(true);

                QCheckBox *checkbox = new QCheckBox(this);
                if (plans_data[i][j].status[k] == 1) {
                    checkbox->setChecked(true);
                } else {
                    checkbox->setChecked(false);
                }
                layout->addWidget(checkbox);
                QWidget *widget = new QWidget(this);
                layout->setSpacing(0);
                layout->setContentsMargins(0,0,0,0);
                widget->setLayout(layout);
                ui->plans->setCellWidget(last_row + 1 + k * 3, j + 3, widget);

                // 连接checkbox连接取消选中事件，用lambda，调用updatePlanChecked
                connect(checkbox, &QCheckBox::clicked, [=](bool state){
                    if (editing_plan_pos.size() > 0) {
                        if (state)
                            checkbox->setChecked(false);
                        else
                            checkbox->setChecked(true);
                        return;
                    }
                    
                    if (!state) {
                        plan_manager->updatePlanChecked(j, i, 0, plans_data[i][j].steps_id[k], QStringList());
                        plan_manager->updatePlan();
                        update_plans();
                        update_loads();
                    } else {
                        plan_manager->updatePlanChecked(j, i, 1, plans_data[i][j].steps_id[k], QStringList());
                    }
                });

                // btn1连接单击事件，用lambda
                connect(btn1, &QPushButton::clicked, [=](){
                    if (editing_plan_pos.size() > 0) return;
                    QVector<int> old_info;
                    // int date_id, int row, int status, int step_id
                    old_info << j << i << plans_data[i][j].status[k] << plans_data[i][j].steps_id[k]
                            << plans_data[i][j].nums[k];
                    QString equip_id = plans_data[i][j].equips[k];
                    on_plans_cell_clicked(last_row + 1 + k * 3, j + 3, old_info, 
                        plans_data[i][j].real_costs[k], equip_id);
                });
                
                
                // item = new QTableWidgetItem(QString::number(plans_data[i][j].costs[k]));


                // 放入一个文本按钮
                QPushButton *btn = new QPushButton(this);
                btn->setText(QString::number(plans_data[i][j].costs[k]));
                btn->setFlat(true);
                // 设置背景色
                QColor color = QColor(SqlOP::getInstance()->getColor(plans_data[i][j].equips[k]));
                double testGray = (0.299*color.red() + 0.587*color.green() + 0.114*color.blue()) / 255;
                QColor fcolor = testGray > 0.5? Qt::black : Qt::white;
                btn->setStyleSheet("background-color: " + color.name() + "; color: " + fcolor.name() + ";");
                ui->plans->setCellWidget(last_row + 2 + k * 3, j + 3, btn);

                // 放入一个文本按钮
                // item = new QTableWidgetItem();

                // 连接单击事件，用lambda
                connect(btn, &QPushButton::clicked, [=](){
                    if (editing_plan_pos.size() > 0) return;
                    QVector<int> old_info;
                    // int date_id, int row, int status, int step_id, int num
                    old_info << j << i << plans_data[i][j].status[k] << plans_data[i][j].steps_id[k] 
                        << plans_data[i][j].nums[k];
                    QString equip_id = plans_data[i][j].equips[k];
                    on_plans_cell_clicked(last_row + 1 + k * 3, j + 3, old_info, 
                        plans_data[i][j].real_costs[k], equip_id);
                });

                // QString::number(plans_data[i][j].real_costs[k])
                // 放入一个文本按钮
                btn = new QPushButton(this);
                btn->setText(QString::number(plans_data[i][j].real_costs[k]));
                btn->setFlat(true);
                
                ui->plans->setCellWidget(last_row + 3 + k * 3, j + 3, btn);

                // 连接单击事件，用lambda
                connect(btn, &QPushButton::clicked, [=](){
                    if (editing_plan_pos.size() > 0) return;
                    QVector<int> old_info;
                    // int date_id, int row, int status, int step_id, int num
                    old_info << j << i << plans_data[i][j].status[k] << plans_data[i][j].steps_id[k] 
                        << plans_data[i][j].nums[k];
                    QString equip_id = plans_data[i][j].equips[k];
                    on_plans_cell_clicked(last_row + 1 + k * 3, j + 3, old_info, 
                        plans_data[i][j].real_costs[k], equip_id);
                });
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

    editing_plan_pos.clear();

}


void MainWindow::on_plans_cell_clicked(int row, int col, QVector<int> old_info, double old_cost, QString equip_id)
{
    editing_plan_pos = QVector<int>() << row << col;

    //QWidget *widget01 = ui->plans->cellWidget(row, col);
    //QWidget *widget02 = ui->plans->cellWidget(row + 1, col);
    //QWidget *widget03 = ui->plans->cellWidget(row + 2, col);


    // 第一行为Grid布局的上面一个spinbox、下面一个对号按钮、一个叉号按钮并列
    // 第二行为水平布局的两个ComboxBox
    // 第三行为水平布局的一个Double spinbox

    QGridLayout *layout1 = new QGridLayout(this);

    QSpinBox *spinbox = new QSpinBox(this);

    spinbox->setMinimum(0);
    spinbox->setMaximum(10000); // TODO: 合法性约束
    spinbox->setSingleStep(1);
    spinbox->setValue(old_info[4]);

    // 一个绿色的对号
    QPushButton *ok = new QPushButton(this);
    ok->setText("✓");
    ok->setStyleSheet("color: green;");

    // 一个红色的叉号
    QPushButton *cancel = new QPushButton(this);
    cancel->setText("✗");
    cancel->setStyleSheet("color: red;");

    layout1->addWidget(spinbox, 0, 0, 1, 2);
    layout1->addWidget(ok, 1, 0);
    layout1->addWidget(cancel, 1, 1);

    layout1->setSpacing(0);
    layout1->setContentsMargins(0,0,0,0);


    QWidget *widget = new QWidget(this);
    widget->setLayout(layout1);
    ui->plans->setCellWidget(row, col, widget);

    
    // 竖直布局的两个ComboBox
    QVBoxLayout *layout2 = new QVBoxLayout(this);
    QComboBox *step = new QComboBox(this);
    QComboBox *equip = new QComboBox(this);
    layout2->addWidget(step);
    layout2->addWidget(equip);
    layout2->setSpacing(0);
    layout2->setContentsMargins(0,0,0,0);
    widget = new QWidget(this);
    widget->setLayout(layout2);
    ui->plans->setCellWidget(row + 1, col, widget);

    QString stock_id = ui->plans->item(row, 0)->text();
    QString work_order = ui->plans->item(row, 1)->text();

    // step取工艺的步骤名称+序号
    QStringList steps = SqlOP::getInstance()->getStepsOrderOfStock(stock_id, work_order);
    for (int i = 0; i < steps.size(); i++) {
        step->addItem(steps[i] + "(" + QString::number(i + 1) + ")");
    }
    // 初始化第editing_plan_info[3]个
    step->setCurrentIndex(old_info[3]);

    // 获取设备编号
    QStringList equips = SqlOP::getInstance()->getEquipmentsOfStep(steps[step->currentIndex()]);
    for (int i = 0; i < equips.size(); i++) {
        equip->addItem(equips[i]);
    }
    // 初始化为equip_id
    equip->setCurrentText(equip_id);

    connect(step, &QComboBox::currentIndexChanged, [=](){
        QStringList equips = SqlOP::getInstance()->getEquipmentsOfStep(steps[step->currentIndex()]);
        equip->clear();
        for (int i = 0; i < equips.size(); i++) {
            equip->addItem(equips[i]);
        }
    });

    // 调整行高
    ui->plans->resizeRowsToContents();


    //layout = new QHBoxLayout(this);
    QDoubleSpinBox *cost = new QDoubleSpinBox(this);
    cost->setValue(old_cost);
    //layout->addWidget(cost);
    //widget = new QWidget(this);
    //widget->setLayout(layout);
    ui->plans->setCellWidget(row + 2, col, cost);

    connect(ok, &QPushButton::clicked, [=](){
        // QVector<int> old_info;
        // int date_id, int row, int status, int step_id

        int date_id = old_info[0];
        int stock_row = old_info[1];
        int step_id = old_info[3];

        plan_manager->updatePlanChecked(date_id, stock_row, 1, step_id,
                                        QStringList() << QString::number(step->currentIndex()) << equip->currentText() <<
                                            QString::number(spinbox->value()) << QString::number(cost->value()));

        // 更新表格
        plan_manager->updatePlan();
        update_plans();
        // 更新负荷
        update_loads();

    });

    connect(cancel, &QPushButton::clicked, [=](){
        /*
        // 恢复
        ui->plans->removeCellWidget(row, col);
        ui->plans->removeCellWidget(row + 1, col);
        ui->plans->removeCellWidget(row + 2, col);
        ui->plans->setCellWidget(row, col, widget01);
        ui->plans->setCellWidget(row + 1, col, widget02);
        ui->plans->setCellWidget(row + 2, col, widget03);

        // editing_plan_info.clear();
        editing_plan_pos.clear();*/
        update_plans();
    });
}

void MainWindow::update_loads()
{
    QDate beg = plan_manager->getBeg();
    QDate end = plan_manager->getEnd();
    int diff = beg.daysTo(end);

    // 关闭表格行头、列头
    ui->loads->verticalHeader()->setVisible(false);
    // ui->loads->horizontalHeader()->setVisible(false);

    // 清空表格
    ui->loads->setRowCount(0);
    ui->loads->setColumnCount(plans_data[0].size() + 2);

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


void MainWindow::on_btn_update_plan_clicked()
{
    // DEGUB
    if (plan_manager) return;

    QString plan_id = "202503";
 /*   QVector<QStringList> stocks = SqlOP::getInstance()->getStocks();
    // 取存货编号和工单
    QVector<QStringList> stock_info = QVector<QStringList>();
    for (int i = 0; i < stocks.size(); i++) {
        stock_info << (QStringList() << stocks[i][0] << stocks[i][5]);
    }

    QString beg = "2025-03-01";
    QString end = "2025-03-31";
    QStringList dates;
    dates << beg << end;
    SqlOP::getInstance()->createPlan(plan_id, dates, stock_info);
*/
    plan_manager = new PlanManager(plan_id);

    plan_manager->updatePlan();

    update_plans();
    update_loads();
    
}


