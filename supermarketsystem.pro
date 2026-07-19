#-------------------------------------------------
#
# Project created by QtCreator 2026-07-18T16:36:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = supermarketsystem
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dbhelper.cpp \
    goodsmanagewidget.cpp \
    membermanagewidget.cpp \
    checkoutwidget.cpp \
    statswidget.cpp

HEADERS  += mainwindow.h \
    dbhelper.h \
    goodsmanagewidget.h \
    membermanagewidget.h \
    checkoutwidget.h \
    statswidget.h

FORMS    += mainwindow.ui \
    goodsmanagewidget.ui \
    membermanagewidget.ui \
    checkoutwidget.ui \
    statswidget.ui
QT+=sql
