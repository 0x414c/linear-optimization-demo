#-------------------------------------------------
#
# Project created by QtCreator 2015-09-18T22:28:37
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++14 warn_on #no_keywords

QMAKE_CXXFLAGS += -m64 -march=native \
  -msse -msse2 -msse3 \
  -mfpmath=sse -mieee-fp -malign-double \
  -frounding-math -fsignaling-nans -ffp-contract=off

TARGET = LinearOptimization

TEMPLATE = app

CONFIG(release, debug|release) {
  DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_INFO_OUTPUT #QT_NO_WARNING_OUTPUT
}

DEFINES += EIGEN_MPL2_ONLY FMT_HEADER_ONLY LP_WITH_DEBUG_LOG

INCLUDEPATH += $$PWD/lib/boost \
  $$PWD/lib/cppformat \
  $$PWD/lib/eigen \
  $$PWD/lib/qcustomplot

#LIBS += -L$$PWD/lib/cppformat/cppformat/build -lformat

SOURCES += src/main.cxx \
  lib/qcustomplot/qcustomplot/qcustomplot.cpp \
  src/lp/dantzignumericsolver.cxx \
  src/lp/graphicalsolver2d.cxx \
  src/lp/inumericsolver.cxx \
  src/lp/linearfunction.cxx \
  src/lp/linearprogramdata.cxx \
  src/lp/linearprogramsolution.cxx \
  src/lp/linearprogramutils.cxx \
  src/lp/plotdata2d.cxx \
  src/misc/dataconvertors.cxx \
  src/misc/ijsonserializable.cxx \
  src/misc/utils.cxx \
  src/ui/mainwindow.cxx \
  src/ui/numericstyleditemdelegate.cxx \
  src/ui/numericvalidator.cxx \
  src/ui/tablemodel.cxx \
  src/ui/tablemodelstorage.cxx \
  src/ui/tablemodelutils.cxx

HEADERS += src/main.hxx \
  lib/qcustomplot/qcustomplot/qcustomplot.h \
  src/config.hxx \
  src/lp/dantzignumericsolver.hxx \
  src/lp/dantzignumericsolver.txx \
  src/lp/graphicalsolver2d.hxx \
  src/lp/graphicalsolver2d.txx \
  src/lp/inumericsolver.hxx \
  src/lp/inumericsolver.txx \
  src/lp/linearfunction.hxx \
  src/lp/linearprogramdata.hxx \
  src/lp/linearprogramdata.txx \
  src/lp/linearprogramsolution.hxx \
  src/lp/linearprogramsolution.txx \
  src/lp/linearprogramutils.hxx \
  src/lp/linearprogramutils.txx \
  src/lp/plotdata2d.hxx \
  src/lp/simplextableau.hxx \
  src/lp/simplextableau.txx \
  src/math/mathutils.hxx \
  src/math/mathutils.txx \
  src/math/numericlimits.hxx \
  src/math/numericlimits.txx \
  src/math/numerictypes.hxx \
  src/misc/boostextensions.hxx \
  src/misc/dataconvertors.hxx \
  src/misc/dataconvertors.txx \
  src/misc/eigenextensions.hxx \
  src/misc/ijsonserializable.hxx \
  src/misc/utils.hxx \
  src/ui/mainwindow.hxx \
  src/ui/numericstyleditemdelegate.hxx \
  src/ui/numericstyleditemdelegate.txx \
  src/ui/numericvalidator.hxx \
  src/ui/tablemodel.hxx \
  src/ui/tablemodelstorage.hxx \
  src/ui/tablemodelutils.hxx \
  src/ui/tablemodelutils.txx

FORMS += forms/mainwindow.ui

RESOURCES += res/resources.qrc
