#-------------------------------------------------
#
# Project created by QtCreator 2015-09-18T20:28:30
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++14 warn_on

TARGET = LinearOptimization

TEMPLATE = app

INCLUDEPATH += $$PWD/lib/boost \
  $$PWD/lib/eigen \
  $$PWD/lib/qcustomplot

#DEPENDPATH += $$PWD/../../../projects/mylib

SOURCES += src/main.cxx \
  lib/qcustomplot/qcustomplot/qcustomplot.cpp \
  src/dantzignumericsolver.cxx \
  src/dataconvertors.cxx \
  src/ijsonserializable.cxx \
  src/inumericsolver.cxx \
  src/linearfunction.cxx \
  src/linearprogramdata.cxx \
  src/linearprogramdataserializable.cxx \
  src/mainwindow.cxx \
  src/numericvalidator.cxx \
  src/tablewidgetserializable.cxx \
  src/tablewidgetutils.cxx \
  src/utils.cxx \
  src/tablemodel.cxx \
  src/numericstyleditemdelegate.cxx

HEADERS  += src/mainwindow.hxx \
  lib/qcustomplot/qcustomplot/qcustomplot.h \
  src/dantzignumericsolver.hxx \
  src/dataconvertors.hxx \
  src/ijsonserializable.hxx \
  src/inumericsolver.hxx \
  src/linearfunction.hxx \
  src/linearprogramdata.hxx \
  src/linearprogramdataserializable.hxx \
  src/numericvalidator.hxx \
  src/tablewidgetserializable.hxx \
  src/tablewidgetutils.hxx \
  src/tablewidgetutils.txx \
  src/utils.hxx \
  src/tablemodel.hxx \
  src/numericstyleditemdelegate.hxx \
  src/numericstyleditemdelegate.txx

FORMS += forms/mainwindow.ui

RESOURCES += res/resources.qrc
