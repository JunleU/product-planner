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
    Cell(QString equip, int num, double cost, double real_cost, int statu)
        : equip(equip), num(num), cost(cost), real_cost(real_cost), statu(statu) {}
    ~Cell() = default;
    
    QString equip; // 设备编号
    int num; // 加工数量
    double cost; // 加工时间
    double real_cost; // 实际加工时间
    int statu; // 选中状态（默认为未选中-0, 选中-1）
};

class Period
{
public:
    Period() = default;
    ~Period() = default;

    void updateCell(int step_id, QString equip, int num, double cost, double real_cost, int statu)
    {
        cells[step_id] = Cell(equip, num, cost, real_cost, statu);
/* 
        if (remain_nums.size() <= step_id + 1)
            return;
        remain_nums[step_id] -= num;
        remain_nums[step_id + 1] += num;
*/
    }

    void clearUnusedCell()
    {
        for (auto it = cells.begin(); it != cells.end();) {
            if (it.value().statu) {
                it++;
            } else {
                it = cells.erase(it);
            }
        }
    }

    QMap<int, Cell> cells; // 键为工序序号，值为Cell

/*  当前算法暂时不需保存remain_nums
    QVector<int> remain_nums; // 各工序的剩余数量
*/
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

    void updatePlanChecked(int date_id, int row, int status, int step_id, QStringList info = QStringList());

    void savePlan();

    // void changePlan(QString stock_id, QString word_order, int pos, QString step, int num);

    QVector<QVector<Period>> getPlans()
    { return plans; }

    QVector<QStringList> getStocksInfos()
    { return stocks_infos; }

    QVector<QMap<QString, double>> getLoads()
    { return load_manager->getLoads(); }

    QDate getBeg()
    { return beg; }

    QDate getEnd()
    { return end; }

    void setOLevel(int o_level)
    { this->o_level = o_level; }

    int getOLevel()
    { return o_level; }

private:
    QString plan_id;
    QDate beg, end;
    int length;

    int o_level;

    QVector<QVector<Period>> plans; // 每个存货的在每个时间单元的生产信息，按交货期限排序
    QVector<QStringList> stocks_infos;  // 存货编号、工单号、交货期限、工艺名称、计划数量

    LoadManager *load_manager;

};


#endif // PLAN_H
