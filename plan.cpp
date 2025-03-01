#include "plan.h"
#include "sqlop.h"


PlanManager::PlanManager(QString plan_id)
{
    this->plan_id = plan_id;

    // 加载计划
    loadPlan();
}

// 从数据库中加载计划，只在初始化时调用
void PlanManager::loadPlan()
{
    // 先获取起止时间
    QStringList date = SqlOP::getInstance()->getDatesOfPlan(plan_id);
    this->beg = QDate::fromString(date[0], "yyyy-MM-dd");
    this->end = QDate::fromString(date[1], "yyyy-MM-dd");

    // 半天为一个单位，故length为end-beg的两倍
    this->length = beg.daysTo(end) * 2;

    // 初始化plans
    this->stocks_infos = SqlOP::getInstance()->getStocksOfPlan(plan_id);
    this->plans.resize(stocks_infos.size());
    for (int i = 0; i < stocks_infos.size(); i++) {
        this->plans[i].resize(length);
        QVector<QStringList> cells_infos = SqlOP::getInstance()->getPlanOfStock(plan_id, stocks_infos[i][0], stocks_infos[i][1]);
        for (int j = 0; j < cells_infos.size(); j++) {
            int pos = cells_infos[j][0].toInt();
            plans[i][pos].nums.append(cells_infos[j][1].toInt());
            plans[i][pos].equips.append(cells_infos[j][2]);
            plans[i][pos].steps_id.append(cells_infos[j][3].toInt());
            plans[i][pos].costs.append(cells_infos[j][4].toDouble());
            plans[i][pos].real_costs.append(cells_infos[j][5].toDouble());
            plans[i][pos].status.append(cells_infos[j][6].toInt());
        }
        // 初始化remain_nums
        int plan_num = stocks_infos[i][4].toInt();
        QStringList steps = SqlOP::getInstance()->getStepsOrderOfTech(stocks_infos[i][3]);
        QVector<int> cur_remain_nums(steps.size() + 1); // 最后留个空，防止bug
        cur_remain_nums[0] = plan_num;
        for (int j = 0; j < steps.size(); j++) {
            cur_remain_nums[j + 1] = 0;
        }
        for (int j = 0; j < length; j++) {
            plans[i][j].remain_nums = cur_remain_nums;
            // 更新remain_nums
            for (int k = 0; k < plans[i][j].nums.size(); k++) {
                cur_remain_nums[plans[i][j].steps_id[k]] -= plans[i][j].nums[k];
                cur_remain_nums[plans[i][j].steps_id[k] + 1] += plans[i][j].nums[k];
            }
        }
    }
    this->load_manager = new LoadManager(this);
}


// 更新一个cell的一个工序的选中状态，info为[step_id, equip_id, num, real_cost]
// 分三种情况：
// 1. status为0，info为空，将plans中的对应项的status改为0（此时须在外部update）
// 2. status为1，info为空，将plans中的对应项的status改为0（此时不需要在外部update）
// 3. status为1，info不为空，将plans中的对应项的status改为1，更新plans中的对应项的数据，（此时须在外部update）
void PlanManager::updatePlanChecked(int date_id, int row, int status, int step_id, QStringList info)
{
    // 如果status为0（未选中），将plans中的对应项的status改为0
    if (status == 0) {
        Cell cell = plans[row][date_id];
        for (int i = 0; i < cell.steps_id.size(); i++) {
            if (cell.steps_id[i] == step_id) {
                plans[row][date_id].status[i] = 0;
                break;
            } 
        }
        return;
    }

    if (info.size() == 0) {
        for (int i = 0; i < plans[row][date_id].steps_id.size(); i++) {
            if (plans[row][date_id].steps_id[i] == step_id) {
                plans[row][date_id].status[i] = 1;
                break;
            }
        } 
        savePlan();
        return;
    }

    // 如果status为1（选中），将plans中的对应项的status改为1, 并更新plans中的对应项的数据
    Cell cell = plans[row][date_id];
    for (int i = 0; i < cell.steps_id.size(); i++) {
        if (cell.steps_id[i] == step_id) {
            plans[row][date_id].steps_id[i] = info[0].toInt();
            plans[row][date_id].status[i] = 1;
            plans[row][date_id].equips[i] = info[1];
            plans[row][date_id].nums[i] = info[2].toInt();
            plans[row][date_id].real_costs[i] = info[3].toDouble();
            return;
        } 
    }
    // 没有找到，新增
    plans[row][date_id].steps_id.append(info[0].toInt());
    plans[row][date_id].equips.append(info[1]);
    plans[row][date_id].nums.append(info[2].toInt());
    plans[row][date_id].real_costs.append(info[3].toDouble());
    plans[row][date_id].status.append(1);

    // cost待计算
    plans[row][date_id].costs.append(0);
}

// TODO:应在合适位置将plans重置
// TODO：当前算法同一时间单位内只进行一个工序，考虑优化
// 更新时日期不允许修改！！！
void PlanManager::updatePlan()
{
    int o_level = 0; // 优化等级

    // 遍历所有stocks
    int stock_num = stocks_infos.size();

    QVector<QVector<int>> help_nums;

    load_manager->clearLoads();

    // 先排一遍选中的项
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
        // 先排一遍选中的项
        for (int j = 0; j < length; j++) {
            // Cell cell = plans[i][j];
            // 清空所有status=0的项
            for (int k = 0; k < plans[i][j].status.size(); k++) {
                if (plans[i][j].status[k] == 0) {
                    plans[i][j].steps_id.erase(plans[i][j].steps_id.begin() + k);
                    plans[i][j].equips.erase(plans[i][j].equips.begin() + k);
                    plans[i][j].nums.erase(plans[i][j].nums.begin() + k);
                    plans[i][j].costs.erase(plans[i][j].costs.begin() + k);
                    plans[i][j].real_costs.erase(plans[i][j].real_costs.begin() + k);
                    plans[i][j].status.erase(plans[i][j].status.begin() + k);
                    k--;
                } 
            }
            // 如果当前cell有选中的工序，则按选中的工序进行加工
            // TODO: num的合法性检查
            int cell_size = plans[i][j].status.size();
            for (int k = 0; k < cell_size; k++) {
                // 计算cost
                int num = plans[i][j].nums[k];
                QString equip_id = plans[i][j].equips[k];
                int step_id = plans[i][j].steps_id[k];

                double rate = load_manager->getRateOfEquip(equip_id);
                double real_load = num * params[step_id] / (rate * 3600);
                plans[i][j].costs[k] = real_load;

                // 更新loads
                load_manager->addLoad(j, equip_id, real_load);

                // 更新nums
                nums[step_id] -= num;
                // nums[step_id + 1] += num;
            }
        }
        // 再排一遍未选中的项
        // nums是排完第一遍后的辅助数组，在时间顺序上不是真实的剩余数量
        help_nums.append(nums);
    }

    for (int i = 0; i < stock_num; i++) {
        // 获取该stocks的工序顺序
        QStringList steps = SqlOP::getInstance()->getStepsOrderOfTech(stocks_infos[i][3]);
        // 各工序待加工数量, 初始化大小为steps.size() + 1
        QVector<int> rnums(steps.size() + 1);
        rnums[0] = stocks_infos[i][4].toInt(); // real_nums

        QVector<int> nums = help_nums[i];

        // 各工序的参数，由QString转换为int
        QVector<int> params(steps.size() + 1);
        for (int j = 0; j < steps.size(); j++) {
            params[j] = SqlOP::getInstance()->
                getTechParam(stocks_infos[i][0], stocks_infos[i][1], steps[j]).toInt();
        }

        int cur = 0; // 当前工序(最早的、未完成工序)下标

        // 遍历所有cell
        
        for (int j = 0; j < length; j++) {
            // 如果当前cell有选中的工序
            if (plans[i][j].status.size() > 0) {
                plans[i][j].remain_nums = rnums;
                for (int k = 0; k < plans[i][j].status.size(); k++) {
                    int step_id = plans[i][j].steps_id[k];
                    rnums[step_id] -= plans[i][j].nums[k];
                    rnums[step_id + 1] += plans[i][j].nums[k];
                    
                    if (rnums[cur] == 0 && cur < steps.size())
                        cur++;

                    // 在这里更新，防止提前加工未到达的工序
                    nums[step_id + 1] += plans[i][j].nums[k];
                }
                if (cur == steps.size())
                    break;
                continue;
            }

            int cur_num = nums[cur]; // 当前工序待加工数量
            if (cur_num <= 0) {
                plans[i][j].remain_nums = rnums;
                continue;
            }

            // 获取当前工序最小负荷的设备编号和负荷
            QPair<QString, double> min_load_equip = load_manager->getMinLoadEquip(j, steps[cur]);

            QString equip_id = min_load_equip.first;
            double min_load = min_load_equip.second;
            double max_load = load_manager->getMaxLoadOfEquip(equip_id);
            
            // 剩余负荷
            double remain_load = max_load - min_load;

            int step_id = cur;
            if (remain_load < MIN_DIFF) {
                if (o_level <= 0)
                    continue;
                
                while (remain_load < MIN_DIFF && step_id < steps.size() - 1) {
                    cur_num = nums[++step_id];
                    min_load_equip = load_manager->getMinLoadEquip(j, steps[step_id]);
                    equip_id = min_load_equip.first;
                    min_load = min_load_equip.second;
                    max_load = load_manager->getMaxLoadOfEquip(equip_id);
                    remain_load = max_load - min_load;
                }

                if (remain_load < MIN_DIFF)
                    continue;
            }
            
            // 设备的稼动率
            double rate = load_manager->getRateOfEquip(equip_id);
            // 参数单位为 秒/支
            // 可以加工的数量
            int can_num = remain_load * 3600 * rate / params[step_id];
            if (can_num > cur_num)
                can_num = cur_num; 
            
            // 加入plans
            plans[i][j].steps_id.append(step_id);
            plans[i][j].equips.append(equip_id);
            plans[i][j].nums.append(can_num);
            // 加工时间 = 加工数量 * 参数 / (设备的稼动率 * 3600)
            double real_load = can_num * params[step_id] / (rate * 3600);
            plans[i][j].costs.append(real_load);
            plans[i][j].real_costs.append(0);
            plans[i][j].status.append(0);

            // 更新remain_nums
            plans[i][j].remain_nums = rnums;

            // 更新rnums
            rnums[step_id] -= can_num;
            rnums[step_id + 1] += can_num;


            // 更新loads
            load_manager->addLoad(j, equip_id, real_load);

            // 更新nums
            nums[step_id] -= can_num;
            nums[step_id + 1] += can_num;

            if (rnums[cur] == 0)
                cur++;

            if (cur == steps.size())
                break;
        }
    } 

    savePlan();
}


void PlanManager::savePlan()
{
    // 保存到数据库
    /*
    计划编号 TEXT,
    时间序号 INTEGER,
    存货编号 TEXT,
    工单号 TEXT,
    计划数量 INTEGER,
    设备编号 TEXT,
    工序序号 INTEGER,
    计划时间 TEXT,
    实际时间 REAL,
    选中状态 INTEGER,
    */
    QVector<QStringList> info;
    for (int i = 0; i < stocks_infos.size(); i++) {
        for (int j = 0; j < length; j++) {
            Cell cell = plans[i][j];
            for (int k = 0; k < cell.steps_id.size(); k++) {
                QStringList row;
                row << QString::number(j) << stocks_infos[i][0] << stocks_infos[i][1]
                    << QString::number(cell.nums[k]) << cell.equips[k] << QString::number(cell.steps_id[k]) 
                    << QString::number(cell.costs[k]) << QString::number(cell.real_costs[k]) 
                    << QString::number(cell.status[k]);
                info.append(row);
            }
        }
    }
    
    SqlOP::getInstance()->updatePlan(plan_id, info);
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

LoadManager::LoadManager(PlanManager *plan_manager)
{
    this->beg = plan_manager->beg;
    this->end = plan_manager->end;

    this->length = beg.daysTo(end) * 2;

    equip_info = SqlOP::getInstance()->getEquipments();

    // 初始化loads
    this->loads.resize(length);
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < equip_info.size(); j++) {
            this->loads[i][equip_info[j][0]] = 0;

        }
    }

    int stock_num = plan_manager->plans.size();
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < stock_num; j++) {
            Cell cell = plan_manager->plans[j][i];
            for (int k = 0; k < cell.equips.size(); k++) {
                this->loads[i][cell.equips[k]] += cell.costs[k]; 
            }
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

void LoadManager::clearLoads()
{
    this->loads.clear();
    this->loads.resize(length);
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < equip_info.size(); j++) {
            this->loads[i][equip_info[j][0]] = 0;
        }
    }
}
