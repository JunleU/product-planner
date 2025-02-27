#include "plan.h"
#include "sqlop.h"


PlanManager::PlanManager(QDate beg, QDate end)
{
    this->beg = beg;
    this->end = end;
    // 半天为一个单位，故length为end-beg的两倍
    this->length = beg.daysTo(end) * 2;
    // 获取所有stocks的存货编号和工单号、交货期限、工艺名称、计划数量
    this->stocks_infos = SqlOP::getInstance()->getStocksByDate("存货编号,工单号,交货期限,工艺,计划数量");

    // 初始化plans
    this->plans.resize(stocks_infos.size());
    for (int i = 0; i < stocks_infos.size(); i++) {
        this->plans[i].resize(length);
    }

    this->load_manager = new LoadManager(beg, end);
}

// TODO:应在合适位置将plans重置
// TODO：当前算法同一时间单位内只进行一个工序，考虑优化
void PlanManager::makePlan()
{
    // 遍历所有stocks
    int stock_num = stocks_infos.size();
    for (int i = 0; i < stock_num; i++) {
        // 获取该stocks的工序顺序
        QStringList steps = SqlOP::getInstance()->getStepsOrderOfTech(stocks_infos[i][3]);
        // 各工序待加工数量, 初始化大小为steps.size() + 1
        QVector<int> nums(steps.size() + 1);
        nums[0] = stocks_infos[i][4].toInt();

        // 各工序的参数，由QString转换为int
        QVector<int> params(steps.size() + 1);
        for (int j = 0; j < steps.size(); j++) {
            params[j] = SqlOP::getInstance()->
                getTechParam(stocks_infos[i][0], stocks_infos[i][1], steps[j]).toInt();
        }

        int cur = 0; // 当前工序(最早的、未完成工序)下标

        // 遍历所有cell
        for (int j = 0; j < length; j++) {
            int cur_num = nums[cur]; // 当前工序待加工数量
            // 获取当前工序最小负荷的设备编号和负荷
            QPair<QString, double> min_load_equip = load_manager->getMinLoadEquip(j, steps[cur]);

            QString equip_id = min_load_equip.first;
            double min_load = min_load_equip.second;
            double max_load = load_manager->getMaxLoadOfEquip(equip_id);
            
            // 剩余负荷
            double remain_load = max_load - min_load;
            if (remain_load < MIN_DIFF)
                continue;
            
            // 设备的稼动率
            double rate = load_manager->getRateOfEquip(equip_id);
            // 参数单位为 秒/支
            // 可以加工的数量
            int can_num = remain_load * 3600 * rate / params[cur];
            if (can_num > cur_num)
                can_num = cur_num; 
            
            // 加入plans
            plans[i][j].steps.append(steps[cur]);
            plans[i][j].equips.append(equip_id);
            plans[i][j].nums.append(can_num);
            // 加工时间 = 加工数量 * 参数 / (设备的稼动率 * 3600)
            double real_load = can_num * params[cur] / (rate * 3600);
            plans[i][j].costs.append(real_load);

            // 更新loads
            load_manager->addLoad(j, equip_id, real_load);

            // 更新nums
            nums[cur] -= can_num;
            nums[cur + 1] += can_num;
            if (nums[cur] == 0)
                cur++;

            if (cur == steps.size())
                break;
        }
    } 
}



LoadManager::LoadManager(QDate beg, QDate end)
{
    this->beg = beg;
    this->end = end;
    
    // 半天为一个单位，故length为end-beg的两倍
    this->length = beg.daysTo(end) * 2;

    equip_info = SqlOP::getInstance()->getEquipments();

    // 初始化loads
    this->loads.resize(length);
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < equip_info.size(); j++) {
            this->loads[i][equip_info[j][0]] = 0;
        }
    }
}


QPair<QString, double> LoadManager::getMinLoadEquip(int pos, QString step)
{
    // 获取所有设备编号
    QStringList equip_ids = SqlOP::getInstance()->getEquipmentsOfStep(step);
    
    // 遍历所有设备编号
    double min_load = 100000000;
    QString min_equip_id;
    for (int i = 0; i < equip_ids.size(); i++) {
        double load = this->loads[pos][equip_ids[i]];
        if (load < min_load) {
            min_load = load;
            min_equip_id = equip_ids[i];
        }
    }

    return QPair<QString, double>(min_equip_id, min_load);
}


double LoadManager::getMaxLoadOfEquip(QString equip_id)
{
    // 获取设备的最大负荷
    for (int i = 0; i < equip_info.size(); i++) {
        if (equip_info[i][0] == equip_id) {
            return equip_info[i][4].toDouble();
        }
    }
    return 0;
}

double LoadManager::getRateOfEquip(QString equip_id)
{
    // 获取设备的稼动率
    for (int i = 0; i < equip_info.size(); i++) {
        if (equip_info[i][0] == equip_id) {
            return equip_info[i][2].toDouble();
        }
    }
    return 0;
}


void LoadManager::addLoad(int pos, QString equip, double load)
{
    this->loads[pos][equip] += load;
}
