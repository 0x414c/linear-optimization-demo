#-------------------------------------------------
#
# Project created by QtCreator 2015-09-18T20:28:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++14 warn_on

TARGET = LinearOptimization

TEMPLATE = app


SOURCES += main.cxx\
    mainwindow.cxx \
    numericvalidatordelegate.cxx \
    numericvalidator.cxx \
    tablewidgetutils.cxx \
    tablewidgetutils.txx \
    qcustomplot/qcustomplot.cpp

HEADERS  += mainwindow.hxx \
    numericvalidatordelegate.hxx \
    numericvalidator.hxx \
    tablewidgetutils.hxx \
    qcustomplot/qcustomplot.h

FORMS    += mainwindow.ui
