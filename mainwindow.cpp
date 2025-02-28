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
    update_stocks();

    //checkTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}



