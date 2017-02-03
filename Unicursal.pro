#-------------------------------------------------
#
# Project created by QtCreator 2016-12-30T11:15:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Unicursal
TEMPLATE = app


SOURCES += main.cpp\
        gamewindow.cpp \
    unicursalalgorithm.cpp

HEADERS  += gamewindow.h \
    unicursalalgorithm.h \
    commonref.h \
    nodecommonref.h \
    configure.h

RESOURCES += \
    icon.qrc
