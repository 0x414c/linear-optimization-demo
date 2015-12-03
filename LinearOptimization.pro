#-------------------------------------------------
#
# Project created by QtCreator 2015-09-18T22:28:37
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++14 warn_on

QMAKE_CXX = ccache g++

QMAKE_CXXFLAGS += -march=native -msse -msse2 -msse3 -mfpmath=sse -m64 \
  -mieee-fp -mno-fp-ret-in-387 -mno-fancy-math-387 -malign-double \
  -ffp-contract=off -ffloat-store -frounding-math -fsignaling-nans

QMAKE_CXXFLAGS_WARN_ON += -fdiagnostics-color=auto \
  -Wpedantic -Wall -Wextra -Wdouble-promotion -Wformat

TARGET = LinearOptimization

TEMPLATE = app

CONFIG(release, debug|release) {
  DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_INFO_OUTPUT #QT_NO_WARNING_OUTPUT
}

DEFINES += EIGEN_MPL2_ONLY FMT_HEADER_ONLY LP_WITH_DEBUG_LOG LP_WITH_BLAND_RULE

INCLUDEPATH += $$PWD/lib/boost \
  $$PWD/lib/cppformat \
  $$PWD/lib/eigen \
  $$PWD/lib/qcustomplot

#LIBS += -L$$PWD/lib/cppformat/cppformat/build -lformat

SOURCES += \
  lib/qcustomplot/qcustomplot/qcustomplot.cpp \
  src/gui/mainwindow.cxx \
  src/gui/numericstyleditemdelegate.cxx \
  src/gui/numericvalidator.cxx \
  src/gui/simpletablemodel.cxx \
  src/gui/tablemodelstorage.cxx \
  src/gui/tablemodelutils.cxx \
  src/lp/dantzignumericsolver.cxx \
  src/lp/dantzignumericsolvercontroller.cxx \
  src/lp/graphicalsolver2d.cxx \
  src/lp/inumericsolver.cxx \
  src/lp/linearfunction.cxx \
  src/lp/linearprogramdata.cxx \
  src/lp/linearprogrammingutils.cxx \
  src/lp/linearprogramsolution.cxx \
  src/lp/plotdata2d.cxx \
  src/main.cxx \
  src/misc/dataconvertors.cxx \
  src/misc/ijsonserializable.cxx \
  src/misc/utils.cxx \
  src/test/test.cxx

HEADERS += \
  lib/qcustomplot/qcustomplot/qcustomplot.h \
  src/config.hxx \
  src/gui/mainwindow.hxx \
  src/gui/numericstyleditemdelegate.hxx \
  src/gui/numericstyleditemdelegate.txx \
  src/gui/numericvalidator.hxx \
  src/gui/simpletablemodel.hxx \
  src/gui/tablemodelstorage.hxx \
  src/gui/tablemodelutils.hxx \
  src/gui/tablemodelutils.txx \
  src/lp/dantzignumericsolver.hxx \
  src/lp/dantzignumericsolver.txx \
  src/lp/dantzignumericsolver_fwd.hxx \
  src/lp/dantzignumericsolvercontroller.hxx \
  src/lp/dantzignumericsolvercontroller_fwd.hxx \
  src/lp/graphicalsolver2d.hxx \
  src/lp/graphicalsolver2d.txx \
  src/lp/inumericsolver.hxx \
  src/lp/inumericsolver.txx \
  src/lp/linearfunction.hxx \
  src/lp/linearprogramdata.hxx \
  src/lp/linearprogramdata.txx \
  src/lp/linearprogrammingutils.hxx \
  src/lp/linearprogrammingutils.txx \
  src/lp/linearprogramsolution.hxx \
  src/lp/linearprogramsolution.txx \
  src/lp/optimizationgoaltype.hxx \
  src/lp/plotdata2d.hxx \
  src/lp/simplextableau.hxx \
  src/lp/simplextableau.txx \
  src/lp/simplextableau_fwd.hxx \
  src/lp/solutionphase.hxx \
  src/lp/solutiontype.hxx \
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
  src/test/test.hxx

FORMS += forms/mainwindow.ui

RESOURCES += res/resources.qrc
