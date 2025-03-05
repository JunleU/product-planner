/*
 TODO: 增加休息制度
*/
CREATE TABLE equipments (
    设备编号 TEXT PRIMARY KEY,
    设备名称 TEXT,
    稼动率 REAL,
    状态 TEXT,
    最大负荷 REAL,
    标记颜色 TEXT
);

CREATE TABLE steps (
    工序名称 TEXT,
    设备编号 INTEGER
);

CREATE TABLE techs (
    工艺名称 TEXT,
    工序名称 TEXT,
    次序 INTEGER
);

CREATE TABLE stocks (
    存货编号 TEXT,
    存货全名 TEXT,
    规格 TEXT,
    型号 TEXT,
    工艺 TEXT,
    工单号 TEXT,
    计划数量 INTEGER,
    交货期限 TEXT,
    PRIMARY KEY (存货编号, 工单号)
);

CREATE TABLE tech_params (
    存货编号 TEXT,
    工单号 TEXT,
    工序名称 TEXT,
    参数值 REAL,
    PRIMARY KEY (存货编号, 工单号, 工序名称)
);

CREATE TABLE plans (
    计划编号 TEXT PRIMARY KEY,
    起始日期 TEXT,
    结束日期 TEXT
);

CREATE TABLE stocks_of_plans (
    计划编号 TEXT,
    存货编号 TEXT,
    工单号 TEXT
);

/* 计划单元 */
/*
主键：计划编号（命名）、时间序号、存货编号、工单号、工序名称
其他属性：起始日期、结束日期、计划数量、设备编号、实际时间、状态（选中、未选中）
*/
CREATE TABLE cells_of_plans {
    计划编号 TEXT,
    时间序号 INTEGER,
    存货编号 TEXT,
    工单号 TEXT,
    计划数量 INTEGER,
    设备编号 TEXT,
    工序序号 INTEGER,
    计划时间 TEXT,
    实际时间 REAL,
    PRIMARY KEY (计划编号, 时间序号, 存货编号, 工单号, 工序序号)
};

