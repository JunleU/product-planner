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
