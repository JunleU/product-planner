#ifndef PLAN_H
#define PLAN_H

#include <QString>
#include <QVector>
#include <QDate>
#include <QPair>
#include <QMap>

#define MIN_DIFF 3


// 半天为一个时间单元，cell中包含了某一存货在某一单元的所有生产信息
class Cell
{
public:
    Cell() = default;
    ~Cell() = default;
    
    QVector<int> steps_id; // 工序时间序号
    QVector<QString> equips; // 设备编号
    QVector<int> nums; // 加工数量
    QVector<double> costs; // 加工时间
    QVector<double> real_costs; // 实际加工时间

    QVector<int> status; // 选中状态（默认为未选中-0, 选中-1）
    QVector<int> remain_nums; // 存储当前单元开始前各工序的待加工数量
};


class PlanManager;

// 负荷管理
class LoadManager
{
public:
    LoadManager(QDate beg, QDate end);

    LoadManager(PlanManager *plan_manager);

    ~LoadManager() = default;

    // 获取某时间单位某工序最小负荷的设备编号和负荷
    QPair<QString, double> getMinLoadEquip(int pos, QString step);

    double getMaxLoadOfEquip(QString equip_id);

    double getRateOfEquip(QString equip_id);
    
    void addLoad(int pos, QString equip, double load);

    void clearLoads();

    QVector<QMap<QString, double>> getLoads()
    { return loads; }

private:
    QDate beg, end;
    int length;

    QVector<QStringList> equip_info;
    
    QVector<QMap<QString, double>> loads; // 每个单位时间的每个设备的负荷
};


// 计划管理
class PlanManager
{
friend class LoadManager;

public:
    PlanManager(QString plan_id);
    ~PlanManager()
    {
        delete load_manager; 
    }

    void loadPlan();

    void updatePlan();

    void updatePlanChecked(int date_id, int row, int status, int step_id, QStringList info);

    void savePlan();

    // void changePlan(QString stock_id, QString word_order, int pos, QString step, int num);

    QVector<QVector<Cell>> getPlans()
    { return plans; }

    QVector<QStringList> getStocksInfos()
    { return stocks_infos; }

    QVector<QMap<QString, double>> getLoads()
    { return load_manager->getLoads(); }

    QDate getBeg()
    { return beg; }

    QDate getEnd()
    { return end; }

private:
    QString plan_id;
    QDate beg, end;
    int length;

    QVector<QVector<Cell>> plans; // 每个存货的在每个时间单元的生产信息，按交货期限排序
    QVector<QStringList> stocks_infos;  // 存货编号、工单号、交货期限、工艺名称、计划数量

    LoadManager *load_manager;

};


#endif // PLAN_H
