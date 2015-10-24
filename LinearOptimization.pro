#-------------------------------------------------
#
# Project created by QtCreator 2015-09-18T22:28:33
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++14 warn_on

TARGET = LinearOptimization

TEMPLATE = app

CONFIG(release, debug|release) {
  DEFINES += QT_NO_DEBUG_OUTPUT
}

DEFINES += EIGEN_MPL2_ONLY

INCLUDEPATH += $$PWD/lib/boost \
  $$PWD/lib/eigen \
  $$PWD/lib/qcustomplot

SOURCES += src/main.cxx \
  lib/qcustomplot/qcustomplot/qcustomplot.cpp \
  src/lp/dantzignumericsolver.cxx \
  src/lp/graphicalsolver2d.cxx \
  src/lp/inumericsolver.cxx \
  src/lp/linearfunction.cxx \
  src/lp/linearprogramdata.cxx \
  src/lp/linearprogramsolution.cxx \
  src/lp/plotdata2d.cxx \
  src/math/mathutils.cxx \
  src/misc/dataconvertors.cxx \
  src/misc/ijsonserializable.cxx \
  src/misc/utils.cxx \
  src/ui/mainwindow.cxx \
  src/ui/numericstyleditemdelegate.cxx \
  src/ui/numericvalidator.cxx \
  src/ui/tablemodel.cxx \
  src/ui/tablemodelstorage.cxx \
  src/ui/tablemodelutils.cxx

HEADERS  += src/ui/mainwindow.hxx \
  lib/qcustomplot/qcustomplot/qcustomplot.h \
  src/lp/dantzignumericsolver.hxx \
  src/lp/dantzignumericsolver.txx \
  src/lp/graphicalsolver2d.hxx \
  src/lp/inumericsolver.hxx \
  src/lp/linearfunction.hxx \
  src/lp/linearprogramdata.hxx \
  src/lp/linearprogramdata.txx \
  src/lp/linearprogramsolution.hxx \
  src/lp/linearprogramutils.hxx \
  src/lp/plotdata2d.hxx \
  src/math/mathutils.hxx \
  src/misc/dataconvertors.hxx \
  src/misc/ijsonserializable.hxx \
  src/misc/utils.hxx \
  src/ui/numericstyleditemdelegate.hxx \
  src/ui/numericstyleditemdelegate.txx \
  src/ui/numericvalidator.hxx \
  src/ui/tablemodel.hxx \
  src/ui/tablemodelstorage.hxx \
  src/ui/tablemodelutils.hxx \
  src/ui/tablemodelutils.txx

FORMS += forms/mainwindow.ui

RESOURCES += res/resources.qrc
