#include "sqlop.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QMessageBox>

SqlOP* SqlOP::instance = nullptr;

SqlOP::SqlOP()
{
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        // 建立和SQlite数据库的连接
        db = QSqlDatabase::addDatabase("QSQLITE");
        // 设置数据库文件的名字
        db.setDatabaseName("data.db");
    }

    init();
}

void SqlOP::init()
{
    // 打开数据库
    if (!db.open()) {
        qDebug() << "Error: Failed to connect database." << db.lastError();
    } else {
        qDebug() << "Database: connection ok.";
    }

    // 检查表是否存在
    QSqlQuery query;
    if (!db.tables().contains("equipments")) {
        // 创建表
        // 设备编号,设备名称,稼动率,状态,最大负荷,标记颜色
        query.exec("CREATE TABLE equipments "
                   "(设备编号 TEXT PRIMARY KEY, "
                   "设备名称 TEXT, "
                   "稼动率 REAL, "
                   "状态 TEXT, "
                   "最大负荷 REAL, "
                   "标记颜色 TEXT);");
    }
    if (!db.tables().contains("steps")) {
        query.exec("CREATE TABLE steps "
                   "(工序名称 TEXT, "
                   "设备编号 INTEGER); ");
    }
    if (!db.tables().contains("techs")) {
        query.exec("CREATE TABLE techs "
                   "(工艺名称 TEXT, "
                   "工序名称 TEXT, "
                   "次序 INTEGER); ");
    }
    if (!db.tables().contains("stocks")) {
        // 存货编号,存货全名,规格,型号,工艺,工艺参数,工单号,计划数量,交货期限
        query.exec("CREATE TABLE stocks "
                   "(存货编号 TEXT , "
                   "存货全名 TEXT, "
                   "规格 TEXT, "
                   "型号 TEXT, "
                   "工艺 TEXT, "
                   "工单号 TEXT, "
                   "计划数量 INTEGER, "
                   "交货期限 TEXT, "
                   "PRIMARY KEY (存货编号, 工单号) ); ");
    }
    // 工艺参数
    if (!db.tables().contains("tech_params")) {
        query.exec("CREATE TABLE tech_params "
                   "(存货编号 TEXT, "
                   "工单号 TEXT, "
                   "工序名称 TEXT, "
                   "参数值 REAL, "
                   "PRIMARY KEY (存货编号, 工单号, 工序名称) ); ");
    }
}


QVector<QStringList> SqlOP::getEquipments()
{
    QSqlQuery q(db);
    QString strSql = "SELECT * FROM equipments ORDER BY 设备编号";
    QVector<QStringList> vec;
    
    bool ret = q.exec(strSql);
    if(!ret) {
       qDebug()<< q.lastError().text();
    } else {
        //一共多少列
        int iCols = q.record().count();
        //每次都读取一行数据
        QStringList l;
        while(q.next()) {
            l.clear();
            for(int i =0;i<iCols;i++) {
                l<<q.value(i).toString();
            }
            vec.push_back(l);
        }
    }
    return vec;
}

QStringList SqlOP::getTechs()
{
    QSqlQuery q(db);
    QString strSql = "SELECT DISTINCT 工艺名称 FROM techs ORDER BY 工艺名称";
    QStringList l;

    bool ret = q.exec(strSql);
    if(!ret) {
       qDebug()<< q.lastError().text(); 
    } else {
        //每次都读取一行数据
        while(q.next()) {
            l<<q.value(0).toString(); 
        } 
    }
    return l;
}

QStringList SqlOP::getSteps()
{
    QSqlQuery q(db);
    QString strSql = "SELECT DISTINCT 工序名称 FROM steps ORDER BY 工序名称";
    QStringList l;

    bool ret = q.exec(strSql);
    if(!ret) {
       qDebug()<< q.lastError().text(); 
    } else {
        //每次都读取一行数据
        while(q.next()) {
            l<<q.value(0).toString(); 
        } 
    }
    return l;
}

QStringList SqlOP::getStepsOrderOfTech(QString tech)
{
    QSqlQuery q(db);
    QString strSql = "SELECT 工序名称 FROM techs WHERE 工艺名称 = '" + tech + "' ORDER BY 次序";
    QStringList l;

    bool ret = q.exec(strSql);
    if(!ret) {
       qDebug()<< q.lastError().text(); 
    } else {
        //每次都读取一行数据
        while(q.next()) {
            l<<q.value(0).toString(); 
        } 
    }
    return l;
}

QStringList SqlOP::getStepsOfTech(QString tech)
{
    QSqlQuery q(db);
    QString strSql = "SELECT DISTINCT 工序名称 FROM techs WHERE 工艺名称 = '" + tech + "' ORDER BY 工序名称";
    QStringList l; 

    bool ret = q.exec(strSql);
    if(!ret) {
       qDebug()<< q.lastError().text(); 
    } else {
        //每次都读取一行数据
        while(q.next()) {
            l<<q.value(0).toString();
        }
    }
    return l;
}

QStringList SqlOP::getEquipmentsOfStep(QString step)
{
    QSqlQuery q(db);
    QString strSql = "SELECT 设备编号 FROM steps WHERE 工序名称 = '" + step + "' ORDER BY 设备编号";
    QStringList l;

    bool ret = q.exec(strSql); 
    if(!ret) {
       qDebug()<< q.lastError().text(); 
    } else {
        //每次都读取一行数据
        while(q.next()) {
            l<<q.value(0).toString();
        }
    }
    return l;
}

QVector<QStringList> SqlOP::getStocks()
{
    QSqlQuery q(db);
    QString strSql = "SELECT * FROM stocks ORDER BY 存货编号";
    QVector<QStringList> vec; 

    bool ret = q.exec(strSql);
    if(!ret) {
       qDebug()<< q.lastError().text();
    } else {
        //一共多少列
        int iCols = q.record().count();
        //每次都读取一行数据
        QStringList l;
        while(q.next()) {
            l.clear();
            for(int i =0;i<iCols;i++) {
                l<<q.value(i).toString();
            }
            vec.push_back(l);
        } 
    }
    return vec;
}

QVector<QStringList> SqlOP::getStocksByDate(QString attrs)
{
    QSqlQuery q(db);
    QString strSql = "SELECT " + attrs + " FROM stocks ORDER BY 交货期限";
    QVector<QStringList> vec;

    bool ret = q.exec(strSql);
    if(!ret) {
       qDebug()<< q.lastError().text();
    } else {
        //一共多少列
        int iCols = q.record().count();
        //每次都读取一行数据
        QStringList l;
        while(q.next()) {
            l.clear();
            for(int i =0;i<iCols;i++) {
                l<<q.value(i).toString();
            }
            vec.push_back(l);
        } 
    }
    return vec;
}

QString SqlOP::getTechParam(QString stock_id, QString work_order, QString step)
{
    QSqlQuery q(db);
    QString strSql = "SELECT 参数值 FROM tech_params WHERE 存货编号 = '" + stock_id + 
        "' AND 工单号 = '" + work_order + "' AND 工序名称 = '" + step + "'";
    QString param; 
    bool ret = q.exec(strSql);
    if(!ret) {
       qDebug()<< q.lastError().text(); 
    } else {
        while(q.next()) {
            param = q.value(0).toString();
        } 
    }
    return param;
}

QString SqlOP::getColor(QString equip_id)
{
    QSqlQuery q(db);
    QString strSql = "SELECT 标记颜色 FROM equipments WHERE 设备编号 = '" + equip_id + "'";
    QString color;
    
    bool ret = q.exec(strSql);
    if(!ret) {
       qDebug()<< q.lastError().text(); 
    } else {
        while(q.next()) {
            color = q.value(0).toString();
        }
    } 
    return color;
}

QStringList SqlOP::getEquipIds()
{
    QSqlQuery q(db);
    QString strSql = "SELECT 设备编号 FROM equipments ORDER BY 设备编号";
    QStringList l;

    bool ret = q.exec(strSql);
    if(!ret) {
       qDebug()<< q.lastError().text(); 
    } else {
        //每次都读取一行数据
        while(q.next()) {
            l<<q.value(0).toString();
        }
    }
    return l; 
}


bool SqlOP::isStockExist(QString stock_id, QString work_order)
{
    QSqlQuery q(db);
    QString strSql = "SELECT * FROM stocks WHERE 存货编号 = '" + stock_id + "' AND 工单号 = '" + work_order + "'";
    bool ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        return false;
    } else {
        return q.next();
    } 
}


bool SqlOP::updateStock(QString stock_id, QString work_order, QStringList stock_info, QVector<int> params)
{
    QSqlQuery q(db);

    // 存货编号 TEXT, 存货全名 TEXT, 规格 TEXT, 型号 TEXT, 工艺 TEXT, 工单号 TEXT, 计划数量 INTEGER, 交货期限 TEXT
    // 0,            1,             2,          3,           4,          5,          6,             7

    // 如果stock_id和work_order为空，则插入
    if(stock_id.isEmpty() && work_order.isEmpty()) {
        if (isStockExist(stock_info[0], stock_info[5])) {
            // 已存在，弹窗
            QMessageBox::information(nullptr, "提示", "已存在该存货");
            return false;
        }

        // 插入
        QString strSql = "INSERT INTO stocks VALUES(";
        for(int i = 0; i < stock_info.size(); i++) {
            if (stock_info[i].isEmpty()) {
                QMessageBox::information(nullptr, "提示", "请填写完整信息");
                return false;
            }
            if (i == 6)
                strSql +=  stock_info[i] + ", ";
            else 
                strSql += "'" + stock_info[i] + "', ";
        }
        // 去掉最后一个逗号
        strSql.chop(2);
        strSql += ")";
        bool ret = q.exec(strSql);
        if(!ret) {
            qDebug()<< q.lastError().text(); 
            QMessageBox::information(nullptr, "提示", "插入失败");
            return false;
        } 

        // 插入tech_params
        QStringList steps = getStepsOfTech(stock_info[4]);
        for(int i = 0; i < steps.size(); i++) {
            strSql = "INSERT INTO tech_params VALUES(";
            strSql += "'" + stock_info[0] + "', '" + stock_info[5] + "', '" + steps[i] + "', " + QString::number(params[i]) + ")";
            ret = q.exec(strSql);
            if(!ret) {
                qDebug()<< q.lastError().text(); 
                QMessageBox::information(nullptr, "提示", "插入失败");
                return false;
            }
        }

        return true;
    }

    // 如果stock_id和work_order不为空，则更新
    if (isStockExist(stock_id, work_order)) {
        // 先删除旧的tech_params
        QString strSql = "DELETE FROM tech_params WHERE 存货编号 = '" + stock_id + "' and 工单号 = '" + work_order + "';";
        bool ret = q.exec(strSql);
        if(!ret) {
            qDebug()<< q.lastError().text(); 
            QMessageBox::information(nullptr, "提示", "操作失败");
            return false;
        }

        // 存货编号 TEXT, 存货全名 TEXT, 规格 TEXT, 型号 TEXT, 工艺 TEXT, 工单号 TEXT, 计划数量 INTEGER, 交货期限 TEXT
        QStringList attr_names = {"存货编号", "存货全名", "规格", "型号", "工艺", "工单号", "计划数量", "交货期限"};
        strSql = "UPDATE stocks SET ";
        for(int i = 0; i < stock_info.size(); i++) {
            if (stock_info[i].isEmpty()) {
                QMessageBox::information(nullptr, "提示", "请填写完整信息");
                return false; 
            }
            strSql += attr_names[i] + " = '" + stock_info[i] + "', ";
        }
        // 去掉最后一个逗号
        strSql.chop(2);
        strSql += " WHERE 存货编号 = '" + stock_id + "' AND 工单号 = '" + work_order + "'";
        ret = q.exec(strSql);
        if(!ret) {
            qDebug()<< q.lastError().text(); 
            QMessageBox::information(nullptr, "提示", "更新失败");
            return false;
        }

        // 再插入tech_params
        QStringList steps = getStepsOfTech(stock_info[4]);
        for(int i = 0; i < steps.size(); i++) {
            strSql = "INSERT INTO tech_params VALUES(";
            strSql += "'" + stock_info[0] + "', '" + stock_info[5] + "', '" + steps[i] + "', " + QString::number(params[i]) + ")";
            ret = q.exec(strSql);
            if(!ret) {
                qDebug()<< q.lastError().text(); 
                QMessageBox::information(nullptr, "提示", "插入失败");
                return false;
            }
        }
    }
    return true;
}

bool SqlOP::deleteStock(QString stock_id, QString work_order)
{
    QSqlQuery q(db);
    QString strSql = "DELETE FROM stocks WHERE 存货编号 = '" + stock_id + "' and 工单号 = '" + work_order + "';";
    bool ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        QMessageBox::information(nullptr, "提示", "删除失败");
        return false;
    } 

    // 删除tech_params
    strSql = "DELETE FROM tech_params WHERE 存货编号 = '" + stock_id + "' and 工单号 = '" + work_order + "';";
    ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        QMessageBox::information(nullptr, "提示", "删除失败");
        return false; 
    } 
    return true;
}


bool SqlOP::isEquipExist(QString equip_id)
{
    QSqlQuery q(db);
    QString strSql = "SELECT * FROM equipments WHERE 设备编号 = '" + equip_id + "'";
    bool ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        return false; 
    } 
    return q.next();
}

bool SqlOP::updateEquip(QString equip_id, QStringList equip_info)
{
    QSqlQuery q(db);

    // 设备编号 TEXT, 设备名称 TEXT, 稼动率 REAL, 状态 TEXT, 最大负荷 REAL, 标记颜色 TEXT
    // 0,            1,             2,          3,           4,          5

    // 如果equip_id为空，操作为插入
    if(equip_id.isEmpty()) {
        if (isEquipExist(equip_info[0])) {
            // 已存在，弹窗
            QMessageBox::information(nullptr, "提示", "已存在该设备");
            return false; 
        }
        // 插入
        QString strSql = "INSERT INTO equipments VALUES(";
        for(int i = 0; i < equip_info.size(); i++) {
            if (equip_info[i].isEmpty()) {
                QMessageBox::information(nullptr, "提示", "请填写完整信息");
                return false;
            }
            if (i == 2 || i == 4)
                strSql +=  equip_info[i] + ", "; 
            else
                strSql += "'" + equip_info[i] + "', ";
        }  
        // 去掉最后一个逗号
        strSql.chop(2);
        strSql += ")";
        bool ret = q.exec(strSql);
        if(!ret) {
            qDebug()<< q.lastError().text();
            QMessageBox::information(nullptr, "提示", "插入失败");
            return false;
        }
        return true;
    } 

    // 如果equip_id不为空，操作为更新
    if (isEquipExist(equip_id)) {
        // 先判断设备编号是否修改
        if (equip_info[0] != equip_id) {
            if (isEquipExist(equip_info[0])) {
                // 已存在，弹窗
                QMessageBox::information(nullptr, "提示", "已存在该设备");
                return false;
            }
        }
        // 设备编号 TEXT, 设备名称 TEXT, 稼动率 REAL, 状态 TEXT, 最大负荷 REAL, 标记颜色 TEXT
        QStringList attr_names = {"设备编号", "设备名称", "稼动率", "状态", "最大负荷", "标记颜色"};
        QString strSql = "UPDATE equipments SET ";
        for(int i = 0; i < equip_info.size(); i++) {
            if (equip_info[i].isEmpty()) {
                QMessageBox::information(nullptr, "提示", "请填写完整信息");
                return false;
            }
            if (i == 2 || i == 4)
                strSql += attr_names[i] + " = " + equip_info[i] + ", ";
            else
                strSql += attr_names[i] + " = '" + equip_info[i] + "', ";
        }
        // 去掉最后一个逗号
        strSql.chop(2);
        strSql += " WHERE 设备编号 = '" + equip_id + "'"; 

        bool ret = q.exec(strSql);
        if(!ret) {
            qDebug()<< q.lastError().text();
            QMessageBox::information(nullptr, "提示", "更新失败");
            return false;
        }

        if (equip_info[0] != equip_id) {
            // 更新工序中的设备编号
            QString strSql = "UPDATE steps SET 设备编号 = '" + equip_info[0] + "' WHERE 设备编号 = '" + equip_id + "';";
            bool ret = q.exec(strSql);  
            if(!ret) {
                qDebug()<< q.lastError().text();
                QMessageBox::information(nullptr, "提示", "更新失败");
                return false; 
            }

            // TODO: 更新plans中的设备编号
        }
    }
    return true;
}

bool SqlOP::deleteEquip(QString equip_id)
{
   // 检查工序中是否有该设备
    QSqlQuery q(db);
    QString strSql = "SELECT * FROM steps WHERE 设备编号 = '" + equip_id + "'";
    bool ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        QMessageBox::information(nullptr, "提示", "删除失败");
        return false;
    }
    if (q.next()) {
        QMessageBox::information(nullptr, "提示", "请先在包含该设备的工序中删除该设备");
        return false;
    } 

    // TODO: 检查plans中是否有该设备

    // 删除设备
    strSql = "DELETE FROM equipments WHERE 设备编号 = '" + equip_id + "';";
    ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        QMessageBox::information(nullptr, "提示", "删除失败");
        return false;
    }
    return true;
}


bool SqlOP::updateStep(QString step_name, QStringList info)
{
    QSqlQuery q(db);

    // 工序名称不能为空
    if (info.size() <= 1 || info[0].isEmpty()) {
        QMessageBox::information(nullptr, "提示", "请填写完整信息");
        return false;
    }
    
    // 如果step_name为空，操作为插入
    if(step_name.isEmpty()) {
        QString new_name = info[0];
        if (isStepExist(new_name)) {
            // 已存在，弹窗
            QMessageBox::information(nullptr, "提示", "已存在该工序");
            return false;
        }
        // 插入
        for (int i = 1; i < info.size(); i++) {
            bool ret = q.exec("INSERT INTO steps VALUES('" + new_name + "', '" + info[i] + "');");
            if(!ret) {
                qDebug()<< q.lastError().text();
                QMessageBox::information(nullptr, "提示", "插入失败");
                return false; 
            }
        }
        return true;
    }

    // 如果step_name不为空，操作为更新
    if (isStepExist(step_name)) {
        // 先判断工序名称是否修改
        if (info[0]!= step_name) {
            if (isStepExist(info[0])) {
                // 已存在，弹窗
                QMessageBox::information(nullptr, "提示", "已存在该工序");
                return false; 
            }  
        }  
        // 删除旧的工序
        QString strSql = "DELETE FROM steps WHERE 工序名称 = '" + step_name + "';";
        bool ret = q.exec(strSql);
        if(!ret) {
            qDebug()<< q.lastError().text();
            QMessageBox::information(nullptr, "提示", "更新失败");
            return false;
        }
        // 插入新的工序
        for (int i = 1; i < info.size(); i++) {
            ret = q.exec("INSERT INTO steps VALUES('" + info[0] + "', '" + info[i] + "');"); 
        }
        if(!ret) {
            qDebug()<< q.lastError().text();
            QMessageBox::information(nullptr, "提示", "更新失败");
            return false; 
        }

        if (info[0]!= step_name) {
            // 更新tech和tech_param 中的工序名称
            QString strSql = "UPDATE techs SET 工序名称 = '" + info[0] + "' WHERE 工序名称 = '" + step_name + "';";
            bool ret = q.exec(strSql);
            if(!ret) {
                qDebug()<< q.lastError().text();
                QMessageBox::information(nullptr, "提示", "更新失败");
                return false;
            }
            strSql = "UPDATE tech_params SET 工序名称 = '" + info[0] + "' WHERE 工序名称 = '" + step_name + "';";
            ret = q.exec(strSql);
            if(!ret) {
                qDebug()<< q.lastError().text();
                QMessageBox::information(nullptr, "提示", "更新失败");
                return false;
            }
        } 
    }
    return true;
}

bool SqlOP::isStepExist(QString step_name)
{
    QSqlQuery q(db);
    QString strSql = "SELECT * FROM steps WHERE 工序名称 = '" + step_name + "'";
    bool ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        return false;
    }
    return q.next();
}


bool SqlOP::deleteStep(QString step_name)
{
    QSqlQuery q(db);
    // 检查tech中是否有该工序
    QString strSql = "SELECT * FROM techs WHERE 工序名称 = '" + step_name + "'";
    bool ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        QMessageBox::information(nullptr, "提示", "删除失败");
        return false;
    }
    if (q.next()) {
        QMessageBox::information(nullptr, "提示", "请先在包含该工序的工艺中删除该工序");
        return false;
    }

    strSql = "DELETE FROM steps WHERE 工序名称 = '" + step_name + "';";
    ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        QMessageBox::information(nullptr, "提示", "删除失败");
        return false; 
    }
    return true;
}


bool SqlOP::updateTech(QString old_name, QString new_name, QStringList info)
{
    QSqlQuery q(db);

    // 工艺名称不能为空 
    if (new_name == "" || info.size() == 0) {
        QMessageBox::information(nullptr, "提示", "请填写完整信息");
        return false; 
    }

    // 如果old_name为空，操作为插入
    if(old_name == "") {
        if (isTechExist(new_name)) {
            // 已存在，弹窗
            QMessageBox::information(nullptr, "提示", "已存在该工艺");
            return false;
        }  
        // 插入
        // 工艺名称 工序 次序
        for (int i = 0; i < info.size(); i++) {
            bool ret = q.exec("INSERT INTO techs VALUES('" + 
                new_name + "', '" + 
                info[i] + 
                "', " + QString::number(i + 1) + ");");
            if(!ret) {
                qDebug()<< q.lastError().text();
                QMessageBox::information(nullptr, "提示", "插入失败");
                return false;
            }

        }
        return true;
    }
    // 如果old_name不为空，操作为更新
    if (isTechExist(old_name)) {
        // 先判断工艺名称是否修改
        if (old_name != new_name) {
            if (isTechExist(new_name)) {
                // 已存在，弹窗
                QMessageBox::information(nullptr, "提示", "已存在该工艺");
                return false;
            }  
        }  

        // 检查stocks中是否有该工艺
        QString strSql = "SELECT * FROM stocks WHERE 工艺 = '" + old_name + "'";
        bool ret = q.exec(strSql);
        if(!ret) {
            qDebug()<< q.lastError().text();
            QMessageBox::information(nullptr, "提示", "更新失败");
            return false;
        }
        bool flag = false;
        QStringList old_steps;
        if (q.next()) {
            // 检查工序集合是否改变
            old_steps = getStepsOfTech(old_name);
            QStringList new_steps = info;
            // 去掉重复的元素
            new_steps.removeDuplicates();
            // 排序
            old_steps.sort();
            new_steps.sort();
            // 比较
            if (old_steps != new_steps) {
                // 让用户确认
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(nullptr, "提示", "该工艺已被使用，"
                    "继续更新将导致该工艺下的所有存货的工艺参数改变，是否继续？", QMessageBox::Yes|QMessageBox::No);
                if (reply == QMessageBox::No) {
                    return false; 
                }
                flag = true;
            }
        }

        // 删除旧的工艺
        strSql = "DELETE FROM techs WHERE 工艺名称 = '" + old_name + "';";
        ret = q.exec(strSql);
        if(!ret) {
            qDebug()<< q.lastError().text();
            QMessageBox::information(nullptr, "提示", "更新失败");
            return false;
        }
        // 插入新的工艺
        for (int i = 0; i < info.size(); i++) {
            ret = q.exec("INSERT INTO techs VALUES('" + new_name + "', '" 
                + info[i] + "', " + QString::number(i + 1) + ");");
            if(!ret) {
                qDebug()<< q.lastError().text();
                QMessageBox::information(nullptr, "提示", "更新失败");
                return false; 
            } 
        }

        if (old_name != new_name) {
            // 更新stocks中的工艺名称
            strSql = "UPDATE stocks SET 工艺 = '" + new_name + "' WHERE 工艺 = '" + old_name + "';";
            ret = q.exec(strSql);
            if(!ret) {
                qDebug()<< q.lastError().text();
                QMessageBox::information(nullptr, "提示", "更新失败");
                return false;
            }

        }

        if (flag) {
            // 选择该工艺下的所有存货，删除这些存货的不在新工艺中的工序的tech_params
            strSql = "DELETE FROM tech_params WHERE "
                "(存货编号, 工单号) IN (SELECT 存货编号, 工单号 FROM stocks WHERE 工艺 = '" + new_name + "') "
                "AND 工序名称 NOT IN (SELECT 工序名称 FROM techs WHERE 工艺名称 = '" + new_name + "');";
            ret = q.exec(strSql);
            if(!ret) {
                qDebug()<< q.lastError().text();
                QMessageBox::information(nullptr, "提示", "更新失败");
                return false;
            }
            // 找到新工艺中的不在旧工艺中的工序
            QStringList steps = info;
            steps.removeDuplicates();
            for (int i = 0; i < steps.size(); i++) {
                if (!old_steps.contains(steps[i])) {
                    // 插入这些工序的tech_params
                    strSql = "INSERT INTO tech_params SELECT 存货编号, 工单号, '"
                         + steps[i] + "', 0 FROM stocks WHERE 工艺 = '" + new_name + "';";
                    ret = q.exec(strSql); 
                    if(!ret) {
                        qDebug()<< q.lastError().text();
                        QMessageBox::information(nullptr, "提示", "更新失败");
                        return false; 
                    }
                } 
            }
        }
        
        
    }
    return true;
}


bool SqlOP::isTechExist(QString tech_name)
{
    QSqlQuery q(db);
    QString strSql = "SELECT * FROM techs WHERE 工艺名称 = '" + tech_name + "'";
    bool ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        return false;
    }
    return q.next();
}

bool SqlOP::deleteTech(QString tech_name)
{
    QSqlQuery q(db);
    // 检查stocks中是否有该工艺
    QString strSql = "SELECT * FROM stocks WHERE 工艺 = '" + tech_name + "'";
    bool ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        QMessageBox::information(nullptr, "提示", "删除失败");
        return false;
    }
    if (q.next()) {
        QMessageBox::information(nullptr, "提示", "请先在该工艺相关存货中替换该工艺");
        return false;
    } 

    // 删除工艺
    strSql = "DELETE FROM techs WHERE 工艺名称 = '" + tech_name + "';";
    ret = q.exec(strSql);
    if(!ret) {
        qDebug()<< q.lastError().text();
        QMessageBox::information(nullptr, "提示", "删除失败");
        return false;
    }
    return true;
}


// DEBUG
void checkTable()
{
    QSqlQuery query;
    query.exec("SELECT * FROM equipments");
    while (query.next()) {
        qDebug() << query.value(0).toString() << query.value(1).toString() << query.value(2).toString() << query.value(3).toString() << query.value(4).toString() << query.value(5).toString();
    }

    query.exec("SELECT * FROM techs");
    while (query.next()) {
        qDebug() << query.value(0).toString() << query.value(1).toString() << query.value(2).toString(); 
    }

    query.exec("SELECT * FROM steps");
    while (query.next()) {
        qDebug() << query.value(0).toString() << query.value(1).toString();
    }
    
    query.exec("SELECT * FROM stocks");
    while (query.next()) {
        qDebug() << query.value(0).toString() << query.value(1).toString() << query.value(2).toString() << query.value(3).toString() << query.value(4).toString() << query.value(5).toString() << query.value(6).toString() << query.value(7).toString(); 
    }

    query.exec("SELECT * FROM tech_params");
    while (query.next()) {
        qDebug() << query.value(0).toString() << query.value(1).toString() << query.value(2).toString() << query.value(3).toString(); 
    }
}

// DEBUG
void insertData()
{
    QSqlQuery query;
    /* equipments
    8#,抛光,1,正常,11,#FCD5B4
    7#,涂层,1,正常,11,#C4D79B
    6#,钝化,1,正常,11,#C4BD97
    5#,段差,0.8,正常,11,#92CDDC
    4#,AGE30,0.8,正常,11,#CC00CC
    3#,SS7,0.8,正常,11,#4F81BD
    2#,北平,0.8,正常,11,#808080
    1#,北平,0.8,正常,11,#C0504D
    */
   /*
    query.exec("INSERT INTO equipments VALUES('8#', '抛光', 1, '正常', 11, '#FCD5B4')");
    query.exec("INSERT INTO equipments VALUES('7#', '涂层', 1, '正常', 11, '#C4D79B')");
    query.exec("INSERT INTO equipments VALUES('6#', '钝化', 1, '正常', 11, '#C4BD97')");
    query.exec("INSERT INTO equipments VALUES('5#', '段差', 0.8, '正常', 11, '#92CDDC')");
    query.exec("INSERT INTO equipments VALUES('4#', 'AGE30', 0.8, '正常', 11, '#CC00CC')");
    query.exec("INSERT INTO equipments VALUES('3#', 'SS7', 0.8, '正常', 11, '#4F81BD')");
    query.exec("INSERT INTO equipments VALUES('2#', '北平', 0.8, '正常', 11, '#808080')");
    query.exec("INSERT INTO equipments VALUES('1#', '北平', 0.8, '正常', 11, '#C0504D')");
*/
    /* techs,steps
    工艺1：         工序A,  工序B,          工序C,  工序D,  工序C
    分别对应设备：   1#,     2#/3#/4#/5#,   6#,     7#,      6#
    工艺2：工序B,工序C,工序D,工序C
    分别对应设备：2#/3#/4#/5#,6#,7#,6#
    */
/*
    query.exec("INSERT INTO techs VALUES('工艺1', '工序A', 1)");
    query.exec("INSERT INTO techs VALUES('工艺1', '工序B', 2)");
    query.exec("INSERT INTO techs VALUES('工艺1', '工序C', 3)");
    query.exec("INSERT INTO techs VALUES('工艺1', '工序D', 4)");
    query.exec("INSERT INTO techs VALUES('工艺1', '工序C', 5)");

    query.exec("INSERT INTO techs VALUES('工艺2', '工序B', 1)");
    query.exec("INSERT INTO techs VALUES('工艺2', '工序C', 2)");
    query.exec("INSERT INTO techs VALUES('工艺2', '工序D', 3)");
    query.exec("INSERT INTO techs VALUES('工艺2', '工序C', 4)");

    query.exec("INSERT INTO steps VALUES('工序A', '1#')");
    query.exec("INSERT INTO steps VALUES('工序B', '2#')");
    query.exec("INSERT INTO steps VALUES('工序B', '3#')");
    query.exec("INSERT INTO steps VALUES('工序B', '4#')");
    query.exec("INSERT INTO steps VALUES('工序B', '5#')");
    query.exec("INSERT INTO steps VALUES('工序C', '6#')");
    query.exec("INSERT INTO steps VALUES('工序D', '7#')");
*/
    /* stocks
    存货编号,存货全名,规格,型号,工艺,工单号,计划数量,交货期限
    HHLP.2R02.8.50.4T-B,高硬高速4刃深沟园鼻刀,2R0.2*2*8*50*D4*4T,2R0.2*8,工艺2,16,500,2025-03-15
    
    query.exec(
        "INSERT INTO stocks VALUES("
        "'HHLP.2R02.8.50.4T-B','高硬高速4刃深沟园鼻刀','2R0.2*2*8*50*D4*4T','2R0.2*8','工艺2','16',500,'2025-03-15'"
        ")");
*/

    // TD.02.2.40.2T	铜刀-2刃深沟平刀黑灰涂层	0.2*0.2*2*40*D4*2T	0.2*2   44	50    2025-03-10
    // 参数 80	216	60 80
    query.exec(
        "INSERT INTO stocks VALUES("
        "'TD.02.2.40.2T','铜刀-2刃深沟平刀黑灰涂层','0.2*0.2*2*40*D4*2T','0.2*2','工艺1','44',250,'2025-03-10'" 
        ")");
    
    /* tech_params
    存货编号,工单号,工序名称,参数值
        HHLP.2R02.8.50.4T-B,16,工序B,100
       

    query.exec("INSERT INTO tech_params VALUES('HHLP.2R02.8.50.4T-B','16','工序B',100)");
    query.exec("INSERT INTO tech_params VALUES('HHLP.2R02.8.50.4T-B','16','工序C',120)");
    query.exec("INSERT INTO tech_params VALUES('HHLP.2R02.8.50.4T-B','16','工序D',110)"); */

    query.exec("INSERT INTO tech_params VALUES('TD.02.2.40.2T','44','工序A',60)");
    query.exec("INSERT INTO tech_params VALUES('TD.02.2.40.2T','44','工序B',80)");
    query.exec("INSERT INTO tech_params VALUES('TD.02.2.40.2T','44','工序C',216)");
    query.exec("INSERT INTO tech_params VALUES('TD.02.2.40.2T','44','工序D',80)");
}
