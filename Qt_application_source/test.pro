#-------------------------------------------------
#
# Project created by QtCreator 2016-04-23T16:19:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imageResizer.cpp \
    lodepng.cpp

HEADERS  += mainwindow.h \
    imageResizer.h \
    lodepng.h

FORMS    += mainwindow.ui
