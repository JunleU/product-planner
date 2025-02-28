#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sqlop.h"
#include "plan.h"

#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>


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
    ui->loads->setRowCount(0);
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


