#ifndef SQLOP_H
#define SQLOP_H

#include <QSqlDatabase>
#include <QDebug>
#include <QString>
#include <QVector>


class SqlOP
{
public:
    SqlOP();

    static SqlOP* instance;
    static SqlOP* getInstance() {
        if(nullptr == instance ) {
            instance = new SqlOP();
        }
        return instance;
    }

    void init();

    // 返回所有设备信息
    QVector<QStringList> getEquipments();

    // 返回所有工艺名称
    QStringList getTechs();

    // 返回所有工序名称
    QStringList getSteps();

    // 返回指定工艺的完整工序顺序
    QStringList getStepsOrderOfTech(QString tech);

    // 返回指定工艺的所有工序名称，不重复
    QStringList getStepsOfTech(QString tech);

    // 返回指定工序的所有设备编号
    QStringList getEquipmentsOfStep(QString step);

    // 返回所有存货信息
    QVector<QStringList> getStocks();

    // 返回指定属性的所有存货信息，按交货期限排序
    QVector<QStringList> getStocksByDate(QString attrs);

    // 返回指定存货的指定工序的参数
    QString getTechParam(QString stock_id, QString work_order, QString step);

    // 返回指定设备的标记颜色
    QString getColor(QString equip_id);

    // 返回所有设备编号
    QStringList getEquipIds();

    // 是否存在指定存货
    bool isStockExist(QString stock_id, QString work_order);

    // 添加或更新库存信息
    bool updateStock(QString stock_id, QString work_order, QStringList info, QVector<int> params);

    // 删除库存信息
    bool deleteStock(QString stock_id, QString work_order);

    // 是否存在指定设备
    bool isEquipExist(QString equip_id);

    // 添加或更新设备信息
    bool updateEquip(QString equip_id, QStringList info);

    // 删除设备信息
    bool deleteEquip(QString equip_id);

    // 添加或更新工序信息
    bool updateStep(QString step_name, QStringList info);

    // 是否存在指定工序
    bool isStepExist(QString step_name);

    // 删除工序信息
    bool deleteStep(QString step_name);

    // 添加或更新工艺信息
    bool updateTech(QString old_name, QString new_name, QStringList info);

    // 是否存在指定工艺
    bool isTechExist(QString tech_name);

    // 删除工艺信息
    bool deleteTech(QString tech_name);


private:
    QSqlDatabase db;

};

// DEBUG
void checkTable();
void insertData();


#endif // SQLOP_H
