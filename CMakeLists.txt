cmake_minimum_required (VERSION 3.3)

## Still experimantal, do not use!

project(LinearOptimization)

set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra \
    -m64 -march=native -msse -msse2 -msse3 \
    -mfpmath=sse -mieee-fp -malign-double \
    -frounding-math -fsignaling-nans -ffp-contract=off"
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

add_definitions(
    -DEIGEN_MPL2_ONLY
    -DFMT_HEADER_ONLY
    -D__WITH_DEBUG_LOG__
)

include_directories(
    lib/boost
    lib/cppformat
    lib/eigen
    lib/qcustomplot
)

set(
    SOURCE_FILES
    lib/qcustomplot/qcustomplot/qcustomplot.cpp
    lib/qcustomplot/qcustomplot/qcustomplot.h
    forms/mainwindow.ui
    res/resources.qrc
    src/lp/dantzignumericsolver.cxx
    src/lp/dantzignumericsolver.hxx
    src/lp/dantzignumericsolver.txx
    src/lp/graphicalsolver2d.cxx
    src/lp/graphicalsolver2d.hxx
    src/lp/graphicalsolver2d.txx
    src/lp/inumericsolver.cxx
    src/lp/inumericsolver.hxx
    src/lp/inumericsolver.txx
    src/lp/linearfunction.cxx
    src/lp/linearfunction.hxx
    src/lp/linearprogramdata.cxx
    src/lp/linearprogramdata.hxx
    src/lp/linearprogramdata.txx
    src/lp/linearprogramsolution.cxx
    src/lp/linearprogramsolution.hxx
    src/lp/linearprogramsolution.txx
    src/lp/linearprogramutils.cxx
    src/lp/linearprogramutils.hxx
    src/lp/linearprogramutils.txx
    src/lp/plotdata2d.cxx
    src/lp/plotdata2d.hxx
    src/lp/simplextableau.hxx
    src/lp/simplextableau.txx
    src/main.cxx
    src/main.hxx
    src/math/mathutils.hxx
    src/math/mathutils.txx
    src/math/numericlimits.hxx
    src/math/numericlimits.txx
    src/math/numerictypes.hxx
    src/misc/boostextensions.hxx
    src/misc/dataconvertors.cxx
    src/misc/dataconvertors.hxx
    src/misc/dataconvertors.txx
    src/misc/eigenextensions.hxx
    src/misc/ijsonserializable.cxx
    src/misc/ijsonserializable.hxx
    src/misc/utils.cxx
    src/misc/utils.hxx
    src/ui/mainwindow.cxx
    src/ui/mainwindow.hxx
    src/ui/numericstyleditemdelegate.cxx
    src/ui/numericstyleditemdelegate.hxx
    src/ui/numericstyleditemdelegate.txx
    src/ui/numericvalidator.cxx
    src/ui/numericvalidator.hxx
    src/ui/tablemodel.cxx
    src/ui/tablemodel.hxx
    src/ui/tablemodelstorage.cxx
    src/ui/tablemodelstorage.hxx
    src/ui/tablemodelutils.cxx
    src/ui/tablemodelutils.hxx
    src/ui/tablemodelutils.txx
)

add_executable(LinearOptimization ${SOURCE_FILES})

#qt5_wrap_ui(UI_HEADERS forms/mainwindow.ui)

#qt5_add_resources(UI_RESOURCES res/resources.qrc)

qt5_use_modules(LinearOptimization Core Gui Widgets PrintSupport)
