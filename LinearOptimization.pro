#-------------------------------------------------------------------------------
#
# Project created by QtCreator 2015-09-18T22:28:37
#
#-------------------------------------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets printsupport
}

CONFIG += c++14 warn_on no_keywords

QMAKE_CXX = ccache g++

CONFIG(release, release|debug) {
  message("Using `release' config")
  QMAKE_CXXFLAGS_RELEASE += -O3 -mtune=generic
} else {
  message("Using `debug' config")
  QMAKE_CXXFLAGS_DEBUG += -O0 -march=native
}

equals(QT_ARCH, x86_64) {
  message("Building for `x86_64' target arch")
  QMAKE_CXXFLAGS += -m64 -mno-fp-ret-in-387
} else {
  equals(QT_ARCH, i386) {
    message("Building for `i386' target arch")
    QMAKE_CXXFLAGS += -m32
  }
}

QMAKE_CXXFLAGS += \
  -msse -msse2 -mfpmath=sse \
  -mieee-fp -mno-fancy-math-387 -malign-double \
  -ffp-contract=off -ffloat-store -frounding-math -fsignaling-nans

QMAKE_CXXFLAGS_WARN_ON += \
  -fdiagnostics-color=auto \
  -Wpedantic -Wall -Wextra -Wdouble-promotion -Wformat

TARGET = LinearOptimization

TEMPLATE = app

VERSION = 0.0.1

win32 {
  message("Windows platform detected")
  ## 1200 : Unicode UTF-16, little endian byte order (BMP of ISO 10646);
  ## available only to managed applications
  RC_CODEPAGE = 1200
  RC_ICONS = $$PWD/res/@256/accessories-calculator.ico
  ## English (United States)
  RC_LANG = 1033
  QMAKE_TARGET_COMPANY = "0x414c!"
  QMAKE_TARGET_PRODUCT = "Linear Programming"
  QMAKE_TARGET_DESCRIPTION = "Linear Programming Demonstrational Tool"
  QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2015  Alexey Gorishny"
}

QMAKE_EXT_CPP += cxx

QMAKE_EXT_H += hxx txx

## NOTE: Temporarily disabled
#CONFIG(release, debug|release) {
#  DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_INFO_OUTPUT QT_NO_WARNING_OUTPUT
#}

## NOTE: This quickly resolves many problems when targeting `i386' arch,
## but can (and will) lead to the performance degradation. For the reference see:
## `http://eigen.tuxfamily.org/dox-devel/group__TopicUnalignedArrayAssert.html'
equals(QT_ARCH, i386) {
#  DEFINES += EIGEN_DONT_ALIGN_STATICALLY
  DEFINES += EIGEN_DONT_VECTORIZE EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
}

DEFINES += \
  EIGEN_MPL2_ONLY \
  FMT_HEADER_ONLY \
  LP_WITH_DEBUG_LOG \
  LP_WITH_BLAND_RULE

#DEFINES += LP_WITH_MULTIPRECISION #TODO: ~! Not yet implemented.

#DEFINES += LP_TEST_MODE

INCLUDEPATH += \
  $$PWD/lib/boost \
  $$PWD/lib/cxx-prettyprint \
  $$PWD/lib/eigen3 \
  $$PWD/lib/fmt \
  $$PWD/lib/qcustomplot

## NOTE: Disabled due to `FMT_HEADER_ONLY' presence
#LIBS += -L$$PWD/lib/fmt/build -lcppformat

SOURCES += \
  lib/qcustomplot/qcustomplot/qcustomplot.cpp \
  src/gui/mainwindow.cxx \
  src/gui/numericstyleditemdelegate.cxx \
  src/gui/numericvalidator.cxx \
  src/gui/stringtablemodel.cxx \
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
  src/gui/stringtablemodel.hxx \
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
  src/test/test.hxx \
  src/globaldefinitions.hxx

FORMS += ui/mainwindow.ui

RESOURCES += res/resources.qrc
