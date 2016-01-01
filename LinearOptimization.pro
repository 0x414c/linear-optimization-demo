#--------------------------------------------------------------------------------------
#
# Project created by QtCreator 2015-09-18T22:28:37
#
#--------------------------------------------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG(release, debug|release) {
  message("Building `release' target" )
}

CONFIG(debug, debug|release) {
  message("Building `debug' target")
}

CONFIG += c++14 warn_on

QMAKE_CXX = ccache g++

equals(QT_ARCH, x86_64) {
  message("Building for `x86_64' target")
  QMAKE_CXXFLAGS += -m64 -mno-fp-ret-in-387
} else {
  message("Building for `i386' target")
  QMAKE_CXXFLAGS += -m32
}

QMAKE_CXXFLAGS += \
  -O0 \
  -march=native -msse -msse2 -msse3 -mfpmath=sse \
  -mieee-fp -mno-fancy-math-387 -malign-double \
  -ffp-contract=off -ffloat-store -frounding-math -fsignaling-nans

QMAKE_CXXFLAGS_WARN_ON += \
  -fdiagnostics-color=auto \
  -Wpedantic -Wall -Wextra -Wdouble-promotion -Wformat

TARGET = LinearOptimization

TEMPLATE = app

VERSION = 0.0.1

win32 {
  RC_ICONS += $$PWD/res/@256/accessories-calculator.ico
  QMAKE_TARGET_COMPANY = "0x414c!"
  QMAKE_TARGET_PRODUCT = "Linear Programming"
  QMAKE_TARGET_DESCRIPTION = "Linear Programming Demonstrational Tool"
  QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2015  Alexey Gorishny"
}

#NOTE: Temporarily disabled
#CONFIG(release, debug|release) {
#  DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_INFO_OUTPUT QT_NO_WARNING_OUTPUT
#}

#NOTE: This quickly resolves many problems when targeting `i386' arch,
#but can (and will) lead to the performance degradation. For the reference see:
#`http://eigen.tuxfamily.org/dox-devel/group__TopicUnalignedArrayAssert.html'
contains(QT_ARCH, i386) {
#  DEFINES += EIGEN_DONT_ALIGN_STATICALLY
  DEFINES += EIGEN_DONT_VECTORIZE EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
}

DEFINES += \
  EIGEN_MPL2_ONLY \
  FMT_HEADER_ONLY \
  LP_WITH_DEBUG_LOG \
  LP_WITH_BLAND_RULE
#  LP_WITH_MULTIPRECISION #TODO: ~! Not yet implemented
#  LP_TEST_MODE

INCLUDEPATH += \
  $$PWD/lib/boost \
  $$PWD/lib/cppformat \
  $$PWD/lib/eigen \
  $$PWD/lib/prettyprint \
  $$PWD/lib/qcustomplot

#NOTE: Disabled due to `FMT_HEADER_ONLY' presence
#LIBS += -L$$PWD/lib/cppformat/cppformat/build -lformat

QMAKE_EXT_CPP += cxx

QMAKE_EXT_H += hxx txx

SOURCES += \
  lib/qcustomplot/qcustomplot/qcustomplot.cpp \
  src/gui/mainwindow.cxx \
  src/gui/numericstyleditemdelegate.cxx \
  src/gui/numericvalidator.cxx \
  src/gui/simpletablemodel.cxx \
  src/gui/tablemodelcollection.cxx \
  src/gui/tablemodelutils.cxx \
  src/lp/linearprogrammingutils.cxx \
  src/main.cxx \
  src/misc/boostqtinterop.cxx \
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
  src/gui/tablemodelcollection.hxx \
  src/gui/tablemodelutils.hxx \
  src/gui/tablemodelutils.txx \
  src/lp/graphicalsolver2d.hxx \
  src/lp/graphicalsolver2d.txx \
  src/lp/isolver.hxx \
  src/lp/isolver.txx \
  src/lp/linearfunction.hxx \
  src/lp/linearprogramdata.hxx \
  src/lp/linearprogramdata.txx \
  src/lp/linearprogrammingutils.hxx \
  src/lp/linearprogrammingutils.txx \
  src/lp/linearprogramsolution.hxx \
  src/lp/linearprogramsolution.txx \
  src/lp/optimizationgoaltype.hxx \
  src/lp/plotdata2d.hxx \
  src/lp/plotdata2d.txx \
  src/lp/simplexsolver.hxx \
  src/lp/simplexsolver.txx \
  src/lp/simplexsolver_fwd.hxx \
  src/lp/simplexsolvercontroller.hxx \
  src/lp/simplexsolvercontroller_fwd.hxx \
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
  src/misc/boostqtinterop.hxx \
  src/misc/dataconvertors.hxx \
  src/misc/dataconvertors.txx \
  src/misc/eigenextensions.hxx \
  src/misc/ijsonserializable.hxx \
  src/misc/utils.hxx \
  src/test/test.hxx

FORMS += forms/mainwindow.ui

RESOURCES += res/resources.qrc
