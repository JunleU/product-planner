#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sqlop.h"

#include <QMessageBox>
#include <QComboBox>
#include <QSpinBox>
#include <QColorDialog>

// #include <QScrollBar>

// 刷新equip表格
void MainWindow::update_equips()
{
    QVector<QStringList> equips = SqlOP::getInstance()->getEquipments();

    // 标题为 "设备编号","设备名称","稼动率","状态","最大负荷（时/半日）"
    ui->equips->setColumnCount(5);
    ui->equips->setHorizontalHeaderLabels(
        QStringList() << "设备编号" << "设备名称" << "稼动率" << "状态" << "最大负荷");
    
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
            item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        }
    }
    // 设置表格列宽
    ui->equips->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 隐藏取消和编辑颜色按钮
    ui->btn_cancel_edit_equip->setVisible(false);
    ui->btn_edit_equip_color->setVisible(false);

    // 启用添加、编辑、和删除按钮
    ui->btn_add_equip->setEnabled(true);
    ui->btn_edit_equip->setEnabled(true);
    ui->btn_del_equip->setEnabled(true);

    // 初始化编辑状态
    editing_equip_row = -1;
    editing_equip_info.clear();
}


void MainWindow::update_techs()
{
    QStringList techs = SqlOP::getInstance()->getTechs();

    // 标题为 "工艺名称" 和 "工序"
    ui->techs->setColumnCount(2);
    ui->techs->setHorizontalHeaderLabels(
        QStringList() << "  工艺名称  " << "  工序  ");

    // 关闭表格行头
    ui->techs->verticalHeader()->setVisible(false);

    // 清空表格
    ui->techs->setRowCount(0);

    // 填充表格
    for (int i = 0; i < techs.size(); i++) {
        ui->techs->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem(techs[i]);
        ui->techs->setItem(i, 0, item); 
        
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

        // 在单元格右侧加入工序
        QStringList steps = SqlOP::getInstance()->getStepsOrderOfTech(techs[i]);
        QString step_str;
        for (int j = 0; j < steps.size(); j++) {
            step_str += steps[j] + " >> ";
        }
        step_str = step_str.left(step_str.size() - 3);
        item = new QTableWidgetItem(step_str);
        ui->techs->setItem(i, 1, item);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    } 

    // 设置表格列宽, 第一列自适应
    ui->techs->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->techs->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // 隐藏取消按钮
    ui->btn_cancel_tech->setVisible(false);

    // 启用添加、编辑、和删除按钮
    ui->btn_add_tech->setEnabled(true);
    ui->btn_edit_tech->setEnabled(true);
    ui->btn_del_tech->setEnabled(true);

    // 初始化编辑状态
    
    editing_tech_row = -1;
    editing_tech_name = "";
    editing_tech_info.clear();
}

void MainWindow::update_steps()
{
    QStringList steps = SqlOP::getInstance()->getSteps();

    // 标题为 "工序" 和 "设备"
    ui->steps->setColumnCount(2);
    ui->steps->setHorizontalHeaderLabels(
        QStringList() << "  工序名称 " << "  设备  ");

    // 关闭表格行头
    ui->steps->verticalHeader()->setVisible(false);

    // 清空表格
    ui->steps->setRowCount(0);

    // 填充表格
    for (int i = 0; i < steps.size(); i++) {
        ui->steps->insertRow(i);
        QTableWidgetItem *item = new QTableWidgetItem(steps[i]);
        ui->steps->setItem(i, 0, item); 
        
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled); 

        // 在单元格右侧加入设备
        QStringList equips = SqlOP::getInstance()->getEquipmentsOfStep(steps[i]);
        /*
        QString equip_str;
        for (int j = 0; j < equips.size(); j++) {
            equip_str += equips[j] + "/";
        }
        equip_str = equip_str.left(equip_str.size() - 1);
        item = new QTableWidgetItem(equip_str);
        ui->steps->setItem(i, 1, item);
        */
        // 将每个设备放入小方块中，背景色为设备的标记颜色

        QWidget *widget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout();
        layout->setSpacing(3);
        layout->setContentsMargins(1,0,0,1);
        for (int j = 0; j < equips.size(); j++) {
            QLabel *label = new QLabel(equips[j]);
            // label宽度比文本宽度大10
            label->setFixedWidth(label->fontMetrics().horizontalAdvance(equips[j]) + 10);
            QColor color = QColor(SqlOP::getInstance()->getColor(equips[j]));
            double testGray = (0.299*color.red() + 0.587*color.green() + 0.114*color.blue()) / 255;
            QColor fcolor = testGray > 0.5? Qt::black : Qt::white;
            // label设置为圆角
            label->setStyleSheet("border-radius: 4px;background-color: " + color.name() + ";color: " + fcolor.name() + ";");
            layout->addWidget(label);
        }
        widget->setLayout(layout);
        // layout宽度根据内容自适应
        layout->setSizeConstraint(QLayout::SetFixedSize);
        ui->steps->setCellWidget(i, 1, widget);
        // 调整表格高度
        ui->steps->resizeRowsToContents();

    }

    // 设置表格列宽, 第一列自适应
    ui->steps->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->steps->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // 隐藏取消按钮
    ui->btn_cancel_step->setVisible(false);

    // 启用添加、编辑、和删除按钮
    ui->btn_add_step->setEnabled(true);
    ui->btn_edit_step->setEnabled(true);
    ui->btn_del_step->setEnabled(true);

    // 初始化编辑状态
    editing_step_row = -1;
    editing_step_name = "";
}



void MainWindow::on_btn_add_equip_clicked()
{
    // 添加一行
    ui->equips->insertRow(ui->equips->rowCount());
    // 初始化单元格
    for (int i = 0; i < 5; i++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        ui->equips->setItem(ui->equips->rowCount() - 1, i, item);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    }
    // 将当前行设置为新行
    ui->equips->setCurrentCell(ui->equips->rowCount() - 1, 0);
    on_btn_edit_equip_clicked();
}

void MainWindow::on_btn_edit_equip_clicked()
{
    // 获取选中的行
    int row = ui->equips->currentRow();
    if (row == -1) {
        QMessageBox::warning(this, "提示", "请选择一行");
        return; 
    }

    // 保存当前行的信息
    editing_equip_row = row;
    editing_equip_info.clear();
    for (int i = 0; i < 5; i++) {
        editing_equip_info.append(ui->equips->item(row, i)->text());
    }

    // 禁用添加、编辑、和删除按钮
    ui->btn_add_equip->setEnabled(false);
    ui->btn_edit_equip->setEnabled(false);
    ui->btn_del_equip->setEnabled(false);

    // 显示取消和编辑颜色按钮
    ui->btn_cancel_edit_equip->setVisible(true);
    ui->btn_edit_equip_color->setVisible(true);

    // 前2列可编辑
    for (int i = 0; i < 2; i++) {
        QTableWidgetItem *item = ui->equips->item(row, i);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable); 
    }

    // 第4列为DoubleSpinBox
    QDoubleSpinBox *spinBox = new QDoubleSpinBox();
    spinBox->setRange(0, 1);
    spinBox->setSingleStep(0.01);
    double rate = 1;
    if (!editing_equip_info[2].isEmpty())
        rate = editing_equip_info[2].toDouble();
    spinBox->setValue(rate);
    ui->equips->setCellWidget(row, 2, spinBox);

    // 第4列为QComboBox
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("正常");
    comboBox->addItem("异常");
    comboBox->setCurrentText(editing_equip_info[3]);
    ui->equips->setCellWidget(row, 3, comboBox);

    // 第5列为SpinBox, 0~12
    QSpinBox *spinBox2 = new QSpinBox();
    spinBox2->setRange(0, 12);
    spinBox2->setSingleStep(1);
    int max_load = 11;
    if (!editing_equip_info[4].isEmpty())
        max_load = editing_equip_info[4].toInt();
    spinBox2->setValue(max_load);
    ui->equips->setCellWidget(row, 4, spinBox2);

    // 调整表格高度
    // ui->equips->resizeRowsToContents();
}


void MainWindow::on_btn_save_equip_clicked()
{
    if (editing_equip_row == -1) {
        return; 
    }

    QString old_equip_id = editing_equip_info[0];

    QStringList equip_info;
    // 前两列直接读
    for (int i = 0; i < 2; i++) {
        equip_info.append(ui->equips->item(editing_equip_row, i)->text());
    }

    // 第3列读DoubleSpinBox
    QDoubleSpinBox *spinBox = (QDoubleSpinBox*)ui->equips->cellWidget(editing_equip_row, 2);
    equip_info.append(QString::number(spinBox->value()));

    // 第4列读QComboBox
    QComboBox *comboBox = (QComboBox*)ui->equips->cellWidget(editing_equip_row, 3);
    equip_info.append(comboBox->currentText());

    // 第5列读SpinBox
    QSpinBox *spinBox2 = (QSpinBox*)ui->equips->cellWidget(editing_equip_row, 4);
    equip_info.append(QString::number(spinBox2->value()));

    // 获取第1列的背景色，16进制表示
    QColor color = ui->equips->item(editing_equip_row, 0)->background().color();
    QString color_str = color.name();
    equip_info.append(color_str);

    // 保存到数据库
    if (!SqlOP::getInstance()->updateEquip(old_equip_id, equip_info)) {
        // QMessageBox::warning(this, "提示", "保存失败");
        return; 
    }

    // 刷新表格
    update_equips();
    update_steps();
}


void MainWindow::on_btn_cancel_edit_equip_clicked()
{
    // 恢复
    editing_equip_row = -1;
    editing_equip_info.clear();

    // 恢复表格
    update_equips();
}

void MainWindow::on_btn_edit_equip_color_clicked()
{
    if (editing_equip_row == -1) {
        return;
    }

    // 获取当前行的第1列的背景色
    QColor color = ui->equips->item(editing_equip_row, 0)->background().color();

    // 弹出颜色对话框
    QColor new_color = QColorDialog::getColor(color, this, "选择颜色");

    // 如果用户点击了取消按钮，返回
    if (!new_color.isValid()) {
        return; 
    }

    // 设置第1、2列的背景色
    ui->equips->item(editing_equip_row, 0)->setBackground(QBrush(new_color));
    ui->equips->item(editing_equip_row, 1)->setBackground(QBrush(new_color));

    // 计算灰度值
    double testGray = (0.299*new_color.red() + 0.587*new_color.green() + 0.114*new_color.blue()) / 255;
    // 根据灰度值设置前景色
    new_color = testGray > 0.5? Qt::black : Qt::white;
    ui->equips->item(editing_equip_row, 0)->setForeground(QBrush(new_color));
    ui->equips->item(editing_equip_row, 1)->setForeground(QBrush(new_color));
}


void MainWindow::on_btn_del_equip_clicked()
{
    int row = ui->equips->currentRow();
    if (row == -1) {
        QMessageBox::warning(this, "提示", "请选择一行");
        return;
    }

    QString equip_id = ui->equips->item(row, 0)->text();
    if (!SqlOP::getInstance()->deleteEquip(equip_id)) {
       return; 
    }
    update_equips();
}


void MainWindow::on_btn_edit_step_clicked()
{
    // 获取选中的行
    int row = ui->steps->currentRow();
    if (row == -1) {
        QMessageBox::warning(this, "提示", "请选择一行");
        return;
    }

    // 保存当前行的信息
    editing_step_row = row;
    editing_step_name = ui->steps->item(row, 0)->text();

    // 禁用添加、编辑、和删除按钮
    ui->btn_add_step->setEnabled(false);
    ui->btn_edit_step->setEnabled(false);
    ui->btn_del_step->setEnabled(false);

    // 显示取消按钮
    ui->btn_cancel_step->setVisible(true);

    // 表格添加一列
    ui->steps->insertColumn(2);
    // 设置标题为空
    ui->steps->setHorizontalHeaderItem(2, new QTableWidgetItem(""));

    // 第row行第1列设置为可编辑
    QTableWidgetItem *item = ui->steps->item(row, 0);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);

    // 第row行第3列设置为一个圆形"+"号按钮和一个QComboBox
    // "+"号按钮
    QPushButton *btn_add = new QPushButton("+");
    btn_add->setFixedSize(15, 15);
    btn_add->setStyleSheet("border-radius: 5px;background-color: #007bff;color: white;");

    // QComboBox
    QComboBox *comboBox = new QComboBox();
    // 获取当前行的第2列的设备
    QStringList equips;
    if (editing_step_name != "")
        equips = SqlOP::getInstance()->getEquipmentsOfStep(editing_step_name);
    // 将不在第2列的设备添加到QComboBox中
    QStringList all_equips = SqlOP::getInstance()->getEquipIds();
    for (int i = 0; i < all_equips.size(); i++) {
        if (!equips.contains(all_equips[i])) {
            comboBox->addItem(all_equips[i]);
        }
    }

    // 将btn和QComboBox并排添加到表格(row,3)中
    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setSpacing(1);
    layout2->setContentsMargins(0,0,0,0);
    layout2->addWidget(comboBox);
    layout2->addWidget(btn_add);
    QWidget *widget2 = new QWidget();
    widget2->setLayout(layout2);
    ui->steps->setCellWidget(row, 2, widget2);


    // 第2列每个方块右边添加一个圆形"x"号按钮
    // removeCellWidget清空原有的Widget
    ui->steps->removeCellWidget(row, 1);
    // 重新添加
    QWidget *widget = new QWidget();
    // 水平布局
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(3);
    layout->setContentsMargins(1,0,0,1);
    for (int i = 0; i < equips.size(); i++) {
        QLabel *label = new QLabel(equips[i]);
        QColor color = QColor(SqlOP::getInstance()->getColor(equips[i]));
        double testGray = (0.299*color.red() + 0.587*color.green() + 0.114*color.blue()) / 255;
        QColor fcolor = testGray > 0.5? Qt::black : Qt::white;
        // label设置为圆角
        label->setStyleSheet("border-radius: 3px;background-color: " + color.name() + ";color: " + fcolor.name() + ";");
        label->setFixedWidth(label->fontMetrics().horizontalAdvance(equips[i]) + 5);
        layout->addWidget(label);

        // 添加一个圆形"x"号按钮
        QPushButton *btn = new QPushButton("-");
        btn->setFixedSize(12, 13);
        btn->setStyleSheet("border-radius: 6px;background-color: #dc3545;color: white;");
        layout->addWidget(btn);
        // 插入空白label
        
        // 连接信号和槽，点击后删除按钮左边的方块和按钮，然后向QComboBox中添加该方块的设备
        connect(btn, &QPushButton::clicked, [=](){
            /// 获取按钮左边的方块
            QLabel *label = (QLabel*)layout->itemAt(layout->indexOf(btn) - 1)->widget();
            // 获取方块的设备
            QString equip = label->text();
            // 将设备添加到QComboBox中
            comboBox->addItem(equip);
            // 对ombox中的值进行排序
            comboBox->model()->sort(0);

            // 删除方块和按钮
            layout->removeWidget(label);
            layout->removeWidget(btn);
            delete label;
            delete btn;
            
            // 调整表格宽度
            // ui->steps->resizeColumnsToContents();
        });
    }
    // 将layout添加到widget中
    widget->setLayout(layout);
    // layout宽度根据内容自适应
    layout->setSizeConstraint(QLayout::SetFixedSize);
    // 将widget添加到表格(row,1)中
    ui->steps->setCellWidget(row, 1, widget);


    // 链接btn信号和槽，点击后将QComboBox中的设备添加到第2列
    connect(btn_add, &QPushButton::clicked, [=](){
        // 获取QComboBox中的设备
        // QComboBox *comboBox = (QComboBox*)ui->steps->cellWidget(row, 2);
        QString equip = comboBox->currentText();
        if (equip == "") {
            return;
        }
        // 将设备添加到第2列
        QLabel *label = new QLabel(equip);
        // label宽度比文本宽度大10
        label->setFixedWidth(label->fontMetrics().horizontalAdvance(equip) + 5); // 

        // 从QComboBox中删除该设备
        comboBox->removeItem(comboBox->findText(equip));

        QColor color = QColor(SqlOP::getInstance()->getColor(equip));
        double testGray = (0.299*color.red() + 0.587*color.green() + 0.114*color.blue()) / 255;
        QColor fcolor = testGray > 0.5? Qt::black : Qt::white;
        // label设置为圆角
        label->setStyleSheet("border-radius: 3px;background-color: " + color.name() + ";color: " + fcolor.name() + ";");
        layout->addWidget(label);
        // 添加一个圆形"x"号按钮
        QPushButton *btn = new QPushButton("-");
        btn->setFixedSize(12, 13);
        btn->setStyleSheet("border-radius: 6px;background-color: #dc3545;color: white;");
        layout->addWidget(btn);
        // 连接信号和槽，点击后删除按钮左边的方块和按钮，然后向QComboBox中添加该方块的设备
        connect(btn, &QPushButton::clicked, [=](){
            /// 获取按钮左边的方块
            QLabel *label = (QLabel*)layout->itemAt(layout->indexOf(btn) - 1)->widget();
            // 获取方块的设备
            QString equip = label->text();
            // 将设备添加到QComboBox中
            comboBox->addItem(equip);
            // 对ombox中的值进行排序
            comboBox->model()->sort(0);
            // 删除方块和按钮
            layout->removeWidget(label);
            layout->removeWidget(btn);
            delete label;
            delete btn;
            // 调整表格宽度
            // ui->steps->resizeColumnsToContents();
        });

        // 调整表格宽度
        // ui->steps->resizeColumnsToContents();

    });
}


void MainWindow::on_btn_add_step_clicked()
{
    // 添加一行
    ui->steps->insertRow(ui->steps->rowCount());
    // 初始化单元格
    for (int i = 0; i < 2; i++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        ui->steps->setItem(ui->steps->rowCount() - 1, i, item); 
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled); 
    }
    // 将当前行设置为新行
    ui->steps->setCurrentCell(ui->steps->rowCount() - 1, 0);
    on_btn_edit_step_clicked();
}


void MainWindow::on_btn_save_step_clicked()
{
    if (editing_step_row == -1) {
        return; 
    }
    QString old_step_name = editing_step_name;

    QStringList step_info;

    // 第1列直接读
    step_info.append(ui->steps->item(editing_step_row, 0)->text());

    // 第2列读layout 中的 QComboBox
    QWidget *widget = ui->steps->cellWidget(editing_step_row, 1);
    QHBoxLayout *layout = (QHBoxLayout*)widget->layout();
    for (int i = 0; i < layout->count() / 2; i++) {
        QLabel *label = (QLabel*)layout->itemAt(i * 2)->widget();
        step_info.append(label->text()); 
    }

    // 保存到数据库
    if (!SqlOP::getInstance()->updateStep(old_step_name, step_info)) {
        return;
    }

    // 刷新表格
    update_steps();
    
}



void MainWindow::on_btn_del_step_clicked()
{
    int row = ui->steps->currentRow();
    if (row == -1) {
        QMessageBox::warning(this, "提示", "请选择一行");
        return;
    }

    QString step_name = ui->steps->item(row, 0)->text();
    if (!SqlOP::getInstance()->deleteStep(step_name)) {
        return; 
    }

    update_steps();
}



void MainWindow::on_btn_cancel_step_clicked()
{
    // 恢复
    editing_step_row = -1;
    editing_step_name = "";

    // 恢复表格
    update_steps();
}



void MainWindow::on_btn_edit_tech_clicked()
{
    // 获取选中的行
    int row = ui->techs->currentRow();
    if (row == -1) {
        QMessageBox::warning(this, "提示", "请选择一行");
        return;
    }

    // 保存当前行的信息
    editing_tech_row = row;
    editing_tech_name = ui->techs->item(row, 0)->text();
    editing_tech_info = SqlOP::getInstance()->getStepsOrderOfTech(editing_tech_name);

    // 禁用添加、编辑、和删除按钮
    ui->btn_add_tech->setEnabled(false);
    ui->btn_edit_tech->setEnabled(false);
    ui->btn_del_tech->setEnabled(false);

    // 显示取消按钮
    ui->btn_cancel_tech->setVisible(true);

    // 插入一行
    ui->techs->insertRow(row + 1);
    // 新行的第2列为一个QComboBox和一个"+"号按钮
    QComboBox *comboBox = new QComboBox();
    // 获取所有的工序
    QStringList all_steps = SqlOP::getInstance()->getSteps();
    // 将所有工序添加到QComboBox中
    for (int i = 0; i < all_steps.size(); i++) {
        comboBox->addItem(all_steps[i]); 
    }
    // 将comboBox和"+"号按钮并排添加到表格(row,1)中
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(1);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(comboBox);
    QPushButton *btn_add = new QPushButton("+");
    btn_add->setFixedSize(20, 20);
    btn_add->setStyleSheet("border-radius: 3px;background-color: #007bff;color: white;");
    layout->addWidget(btn_add);
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    ui->techs->setCellWidget(row + 1, 1, widget);

    

    // 第1列可编辑
    QTableWidgetItem *item = ui->techs->item(row, 0);
    item->setFlags(item->flags() ^ Qt::ItemIsEditable);

    // 第二列设置一个layout, 左侧为label，右侧为一个“-”号按钮
    widget = new QWidget();
    layout = new QHBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    // 左侧为label
    QLabel *label = new QLabel(ui->techs->item(row, 1)->text());
    layout->addWidget(label);
    // 右侧为一个“-”号按钮
    QPushButton *btn = new QPushButton("-");
    btn->setFixedSize(20, 20);
    btn->setStyleSheet("border-radius: 3px;background-color: #dc3545;color: white;");
    layout->addWidget(btn);
    widget->setLayout(layout);
    ui->techs->setCellWidget(row, 1, widget);

    // 绑定信号和槽
    connect(btn_add, &QPushButton::clicked, [=](){
        // 加入editing_tech_info
        editing_tech_info.append(comboBox->currentText());
        label->setText(editing_tech_info.join(" >> "));
        
        // 调整表格宽度
        // ui->techs->resizeColumnsToContents(); 
        // 刷新滚动条
        // ui->techs->verticalScrollBar()->setValue(ui->techs->verticalScrollBar()->maximum());
    });

    connect(btn, &QPushButton::clicked, [=](){
       // 删除editing_tech_info末尾的元素
       if (editing_tech_info.isEmpty())
           return;
       editing_tech_info.removeLast();
       // 重新设置label
       label->setText(editing_tech_info.join(" >> ")); 

        // 调整表格宽度
        // ui->techs->resizeColumnsToContents();
        // 刷新滚动条
        // ui->techs->verticalScrollBar()->setValue(ui->techs->verticalScrollBar()->maximum());
    });
}


void MainWindow::on_btn_add_tech_clicked()
{
    // 添加一行
    ui->techs->insertRow(ui->techs->rowCount());
    // 初始化单元格
    for (int i = 0; i < 2; i++) {
        QTableWidgetItem *item = new QTableWidgetItem("");
        ui->techs->setItem(ui->techs->rowCount() - 1, i, item);
        item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled); 
    } 

    // 将当前行设置为新行
    ui->techs->setCurrentCell(ui->techs->rowCount() - 1, 0);
    on_btn_edit_tech_clicked();
}


void MainWindow::on_btn_save_tech_clicked()
{
    if (editing_tech_row == -1) {
        return;
    }

    QString old_tech_name = editing_tech_name;
    QString new_tech_name = ui->techs->item(editing_tech_row, 0)->text();
    QStringList new_steps = editing_tech_info;

    // 保存到数据库
    if (!SqlOP::getInstance()->updateTech(old_tech_name, new_tech_name, new_steps)) {
        return;
    }

    // 刷新表格
    update_techs();
    update_stocks();

}


void MainWindow::on_btn_cancel_tech_clicked()
{
    // 恢复表格
    update_techs();
}


void MainWindow::on_btn_del_tech_clicked()
{
    int row = ui->techs->currentRow();
    if (row == -1) {
        QMessageBox::warning(this, "提示", "请选择一行");
        return;
    } 

    QString tech_name = ui->techs->item(row, 0)->text();
    if (!SqlOP::getInstance()->deleteTech(tech_name)) {
        return;
    }

    update_techs();
}