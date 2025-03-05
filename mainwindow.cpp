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
    // ui->date_beg->setDate(QDate::currentDate());
    // ui->date_end->setDate(QDate::currentDate());

    // ui->stocks->setEditTriggers(QAbstractItemView::SelectedClicked);
    // ui->stocks->setEditTriggers(QAbstractItemView::NoEditTriggers);

    update_equips();
    update_techs();
    update_steps();
    update_stocks();

    plan_manager = nullptr;

    // 把gb_olel中的3个radiobutton加入olevel_group
    olevel_group = new QButtonGroup(this);
    olevel_group->addButton(ui->rbo1, 0);
    olevel_group->addButton(ui->rbo2, 1);
    olevel_group->addButton(ui->rbo3, 2);

    olevel_group->setExclusive(true); // 设置互斥
    // 设置默认选中第一个按钮
    ui->rbo1->setChecked(true);
    // 连接信号和槽函数
    connect(olevel_group, &QButtonGroup::buttonClicked, this, &MainWindow::olevel_changed);

    update_plan_info("");

    ui->plans->setContextMenuPolicy(Qt::CustomContextMenu);//设置右击菜单
    connect(ui->plans, &QTableWidget::customContextMenuRequested, this, &MainWindow::on_plan_show_menu);

    //checkTable();

}

MainWindow::~MainWindow()
{
    delete ui;
}

