#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sqlop.h"

#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"

//#include <QDebug>


int MainWindow::output_xlsx(QString fileName)
{
    if (fileName.isEmpty())
        return -1;
    
    QXlsx::Document xlsx;

    // 存货数据包括：存货编号、存货全名、规格、型号、工艺、工单号、交货期限、计划数量

    // 负荷数据包括：设备编号、设备名称

    // 日期须对齐，故有7列空余

    // 输出负荷数据
    QXlsx::Format titleFormat;
    titleFormat.setFontBold(true);
    titleFormat.setFontSize(13);
    // 灰底
    titleFormat.setPatternBackgroundColor(QColor(192, 192, 192));
    // 黑字
    titleFormat.setFontColor(QColor(0, 0, 0));
    // 宋体
    titleFormat.setFontName("宋体");

    // 框线
    titleFormat.setBorderColor(QColor("#000000"));
    titleFormat.setBorderStyle(QXlsx::Format::BorderThin);

    // 水平、垂直居中
    titleFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    titleFormat.setVerticalAlignment(QXlsx::Format::AlignVCenter);

    xlsx.write(1, 8, "设备编号", titleFormat);
    xlsx.write(1, 9, "设备名称", titleFormat);
    
    /*
    for (int col = 0; col < ui->loads->columnCount(); ++col) {
        xlsx.write(1, col + 3, ui->loads->horizontalHeaderItem(col)->text());
    }*/
    for (int row = 0; row < ui->loads->rowCount(); ++row) {
        for (int col = 0; col < ui->loads->columnCount(); ++col) {
            QXlsx::Format format;
            // 获取单元格的背景色
            QBrush brush = ui->loads->item(row, col)->background();
            QColor color = brush.color();
            format.setPatternBackgroundColor(color);
            // 获取单元格的字体颜色
            brush = ui->loads->item(row, col)->foreground();
            color = brush.color();
            format.setFontColor(color);
            if (col == 1) {
                // 加粗
                format.setFontBold(true);
            }
            // 宋体
            format.setFontName("宋体");
            format.setFontSize(13);
            // 水平、垂直居中
            format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
            format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
            // 框线
            format.setBorderColor(QColor("#000000"));
            format.setBorderStyle(QXlsx::Format::BorderThin);

            xlsx.write(row + 2, col + 8, ui->loads->item(row, col)->text(), format);
        }
    }

    // 输出计划数据
    int startRow = ui->loads->rowCount() + 3;
    // 存货数据包括：存货编号、存货全名、规格、型号、工艺、工单号、交货期限、计划数量
    QXlsx::Format titleFormat2;
    titleFormat2.setFontBold(true);
    titleFormat2.setFontSize(13);
    // 深一点的灰底
    titleFormat2.setPatternBackgroundColor(QColor(128, 128, 128));
    // 黑字
    titleFormat2.setFontColor(QColor(0, 0, 0));
    // 新細明體
    titleFormat2.setFontName("新細明體");

    // 框线
    titleFormat2.setBorderColor(QColor("#000000"));
    titleFormat2.setBorderStyle(QXlsx::Format::BorderThin);

    // 水平、垂直居中
    titleFormat2.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    titleFormat2.setVerticalAlignment(QXlsx::Format::AlignVCenter);

    xlsx.write(startRow - 1, 1, "存货编号", titleFormat2);
    xlsx.write(startRow - 1, 2, "存货全名", titleFormat2);
    xlsx.write(startRow - 1, 3, "规格", titleFormat2);
    xlsx.write(startRow - 1, 4, "型号", titleFormat2);
    xlsx.write(startRow - 1, 5, "工艺", titleFormat2);
    xlsx.write(startRow - 1, 6, "工单号", titleFormat2);
    xlsx.write(startRow - 1, 7, "交货期限", titleFormat2);
    xlsx.write(startRow - 1, 8, "计划数量", titleFormat2);
    xlsx.write(startRow - 1, 9, "", titleFormat2);
    // 向下合并
    for (int i = 1; i <= 9; ++i) {
        xlsx.mergeCells(QXlsx::CellRange(startRow - 1, i, startRow, i), titleFormat2);
    }

    
    for (int col = 3; col < ui->plans->columnCount(); ++col) {
        QString text = ui->plans->horizontalHeaderItem(col)->text();
        // MM-dd 白班
        // 分两行
        QStringList date = text.split(" ");
        xlsx.write(startRow, col + 7, date[1], titleFormat);
        if (col % 2) {
            // 合并单元格
            xlsx.write(startRow - 1, col + 7, date[0], titleFormat);
            xlsx.mergeCells(QXlsx::CellRange(startRow - 1, col + 7, startRow - 1, col + 8), titleFormat);
        }
        // xlsx.write(startRow - 1, col + 7, ui->plans->horizontalHeaderItem(col)->text(), titleFormat);
    }

    startRow++;

    QString last_stock_id = "";
    QString last_work_order = "";
    int row_num = 1;
    for (int row = 0; row < ui->plans->rowCount(); ++row) {
        QString stock_id = last_stock_id;
        QString work_order = last_work_order;
        if (ui->plans->item(row, 0)) {
            stock_id = ui->plans->item(row, 0)->text();
            work_order = ui->plans->item(row, 1)->text();
        }
        if (stock_id != last_stock_id || work_order != last_work_order) {
            QStringList info = SqlOP::getInstance()->getStockInfo(stock_id, work_order,
                "存货编号, 存货全名, 规格, 型号, 工艺, 工单号, 交货期限, 计划数量");
            QXlsx::Format format;
            
            // 新細明體
            format.setFontName("新細明體");
            format.setFontSize(13);
            
            format.setTextWrap(true);//自动换行

            //框线
            format.setBorderColor(QColor(0, 0, 0));
            format.setBorderStyle(QXlsx::Format::BorderThin);

            // 水平、垂直居中
            format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
            format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
            
            for (int i = 0; i < info.size(); ++i) {
                xlsx.write(row + startRow, i + 1, info[i], format);
            }
            last_stock_id = stock_id;
            last_work_order = work_order;
            if (row > 0) { 
                // 合并单元格 row-row_num+1+startRow~row+startRow
                for (int i = 1; i <= 8; ++i) {
                    // 转换成字母数字行列号，如：“D2:D4"
                    xlsx.mergeCells(QXlsx::CellRange(row - row_num + startRow, i, row + startRow - 1, i), format);
                }
            }
            row_num = 1;
        } else {
            ++row_num;
        }
        for (int col = 2; col < ui->plans->columnCount(); ++col) {
            if (ui->plans->item(row, col)) {
                QXlsx::Format format;
                // 宋体
                format.setFontName("宋体");
                format.setFontSize(13);
                //框线
                format.setBorderColor(QColor(0, 0, 0));
                format.setBorderStyle(QXlsx::Format::BorderThin);

                // 水平、垂直居中
                format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                format.setVerticalAlignment(QXlsx::Format::AlignVCenter);

                // 获取单元格的背景色
                QBrush brush = ui->plans->item(row, col)->background();
                if (!brush.isOpaque()) {
                    xlsx.write(row + startRow, col + 7, ui->plans->item(row, col)->text(), format);
                    continue;
                }
                QColor color = brush.color();
                // 判断颜色是否为透明
                format.setPatternBackgroundColor(color);
                qDebug() << color.name();
                // 获取单元格的字体颜色
                brush = ui->plans->item(row, col)->foreground();
                color = brush.color();
                format.setFontColor(color);
                qDebug() << color.name();
                xlsx.write(row + startRow, col + 7, ui->plans->item(row, col)->text(), format);
            } else if (ui->plans->cellWidget(row, col)) {
                QWidget *widget = ui->plans->cellWidget(row, col);
                QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(widget->layout());
                QPushButton *button = nullptr;
                
                // 背景色
                QXlsx::Format format;
                // 宋体
                format.setFontSize(13);
                //框线
                format.setBorderColor(QColor(0, 0, 0));
                format.setBorderStyle(QXlsx::Format::BorderThin);
                // 水平、垂直居中
                format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                format.setVerticalAlignment(QXlsx::Format::AlignVCenter);

                if (layout) {
                    // QCheckBox *checkbox = qobject_cast<QCheckBox*>(layout->itemAt(1)->widget());
                    button = qobject_cast<QPushButton*>(layout->itemAt(0)->widget());
                    format.setFontName("新細明體");
                } else {
                    button = qobject_cast<QPushButton*>(widget);
                    format.setFontName("宋体");
                }
                if (button) {
                    QColor color = button->palette().color(QPalette::Button);
                    // 判断颜色是否为透明
                    if (color.alpha() == 0) {
                        xlsx.write(row + startRow, col + 7, button->text(), format);
                        continue;
                    }
                    format.setPatternBackgroundColor(color);
                    // 字体颜色
                    color = button->palette().color(QPalette::ButtonText);
                    format.setFontColor(color);
                    xlsx.write(row + startRow, col + 7, button->text(), format);
                }
            } else {
                QXlsx::Format format;
                // 宋体
                format.setFontName("宋体");
                format.setFontSize(13);
                //框线
                format.setBorderColor(QColor(0, 0, 0));
                format.setBorderStyle(QXlsx::Format::BorderThin);
                // 水平、垂直居中
                format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
                format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
                xlsx.write(row + startRow, col + 7, "", format);
            }
        }
    }
    // 合并最后一行
    for (int i = 1; i <= 8; ++i) {
        // 转换成字母数字行列号，如：“D2:D4"
        // 新細明體
        QXlsx::Format format;
        format.setFontName("新細明體");
        format.setFontSize(13);

        //框线
        format.setBorderColor(QColor(0, 0, 0));
        format.setBorderStyle(QXlsx::Format::BorderThin);
        // 水平、垂直居中
        format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        format.setVerticalAlignment(QXlsx::Format::AlignVCenter);
        xlsx.mergeCells(QXlsx::CellRange(ui->plans->rowCount() - row_num + startRow, i, 
            ui->plans->rowCount() + startRow - 1, i), format);
        
    }
    
    // 设置列宽
    xlsx.setColumnWidth(1, 9, 13);
    // 设置行高
    xlsx.setRowHeight(1, startRow - 1, 20);
    xlsx.setRowHeight(startRow, ui->plans->rowCount() + startRow - 1, 18);



    return xlsx.saveAs(fileName) ? 0 : -1;
}
