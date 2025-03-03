QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    checkboxheaderview.cpp \
    dialogmanplans.cpp \
    dialogselectstocks.cpp \
    equip_page.cpp \
    main.cpp \
    mainwindow.cpp \
    output.cpp \
    plan.cpp \
    plan_page.cpp \
    sqlop.cpp \
    stock_page.cpp

HEADERS += \
    checkboxheaderview.h \
    dialogmanplans.h \
    dialogselectstocks.h \
    mainwindow.h \
    output.h \
    plan.h \
    sqlop.h

FORMS += \
    dialogmanplans.ui \
    dialogselectstocks.ui \
    mainwindow.ui

# QXlsx code for Application Qt project

QXLSX_PARENTPATH=./         # current QXlsx path is . (. 表示当前目录)

QXLSX_HEADERPATH=./header/  # 当前目录下的header子目录

QXLSX_SOURCEPATH=./source/  # 当前目录下的source子目录

include(./QXlsx.pri) #.pri文件应该包含了QXlsx库的配置信息

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


