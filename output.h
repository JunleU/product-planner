#ifndef OUTPUT_H
#define OUTPUT_H


#include <QString>
#include <QVector>



// 输出数据到xlsx文件
// filename: 文件名，包括路径；data: 数据；format: 样式； sheetname: 工作表名；title: 标题
// 返回值：0表示成功，-1表示失败
int output_xlsx(QString filename, QVector<QStringList> data, QVector<QVector<int>> format, QString sheetname, QString title);


int output_xlsx(QTableWidget *loads, QTableWidget *plans, QString plan_id);

#endif // OUTPUT_H
