#pragma once

#ifndef CONFIG_HXX
#define CONFIG_HXX


#include <cstdint>
#include <utility>

#include <QColor>
#include <QLocale>
#include <QMap>
#include <QString>

#include "math/numerictypes.hxx"


namespace Config
{
  namespace AppGlobal
  {
    const QString AppName = "Linear Programming";
    const QString OrgName = "0x414c!";
    const QString AppVersion = "0.0.1";
    const QString AppDomain = "www.0x414c.tk";
    const QString AppDisplayName = AppName;
    const QString AppDescription = "Linear Programming Demonstrational Tool";

    //NOTE: Identifier format is: [language[_territory][.codeset][@modifier]]
    constexpr const char* const LocaleIdentifier = "en_US.UTF-8";
    const QLocale Locale(LocaleIdentifier);

    constexpr bool DesktopSettingsAware = true;
  }


  namespace GUI
  {
    using std::make_pair;


    constexpr int ProgramModelsCount = 3;
    constexpr int SimplexModelsCount = 3;

    constexpr int MinConstraints = 1;
    constexpr int MaxConstraints = 16;
    constexpr int DefaultConstraints = 3;

    constexpr int MinVariables = 1;
    constexpr int MaxVariables = 16;
    constexpr int DefaultVariables = 3;

    const QString DefaultDocumentTitle = "Untitled";

    constexpr qreal SelectedPenWidth = 2.5;

    constexpr int FontSize = 9;

    constexpr int ColorMapResolution = 40;

    constexpr int ColorMapLevelsCount = 80;

    constexpr uint8_t ColorMapAlpha = 95;

    //NOTE: Generated w/ `http://tristen.ca/hcl-picker'
    const QMap<double, QColor> ColorMap_BlGrYe = QMap<double, QColor>{
      make_pair(0., QColor( 14,  30,  38, ColorMapAlpha)),
      make_pair(.1, QColor( 18,  50,  57, ColorMapAlpha)),
      make_pair(.2, QColor( 21,  72,  75, ColorMapAlpha)),
      make_pair(.3, QColor( 25,  96,  91, ColorMapAlpha)),
      make_pair(.4, QColor( 38, 119, 102, ColorMapAlpha)),
      make_pair(.5, QColor( 59, 143, 109, ColorMapAlpha)),
      make_pair(.6, QColor( 88, 168, 113, ColorMapAlpha)),
      make_pair(.7, QColor(120, 191, 114, ColorMapAlpha)),
      make_pair(.8, QColor(160, 214, 114, ColorMapAlpha)),
      make_pair(.9, QColor(204, 235, 111, ColorMapAlpha)),
      make_pair(1., QColor(254, 255, 112, ColorMapAlpha))
    };

    const QMap<double, QColor> ColorMap_PuBlGrYe = QMap<double, QColor>{
      make_pair(0.00, QColor( 37,  24,  37, ColorMapAlpha)),
      make_pair(0.05, QColor( 43,  34,  50, ColorMapAlpha)),
      make_pair(0.10, QColor( 48,  45,  64, ColorMapAlpha)),
      make_pair(0.15, QColor( 50,  56,  78, ColorMapAlpha)),
      make_pair(0.20, QColor( 49,  69,  92, ColorMapAlpha)),
      make_pair(0.25, QColor( 46,  82, 104, ColorMapAlpha)),
      make_pair(0.30, QColor( 40,  95, 115, ColorMapAlpha)),
      make_pair(0.35, QColor( 30, 109, 125, ColorMapAlpha)),
      make_pair(0.40, QColor( 16, 123, 132, ColorMapAlpha)),
      make_pair(0.45, QColor(  0, 137, 137, ColorMapAlpha)),
      make_pair(0.50, QColor(  1, 151, 140, ColorMapAlpha)),
      make_pair(0.55, QColor( 24, 165, 140, ColorMapAlpha)),
      make_pair(0.60, QColor( 48, 178, 138, ColorMapAlpha)),
      make_pair(0.65, QColor( 73, 191, 135, ColorMapAlpha)),
      make_pair(0.70, QColor( 99, 204, 129, ColorMapAlpha)),
      make_pair(0.75, QColor(127, 216, 123, ColorMapAlpha)),
      make_pair(0.80, QColor(156, 227, 115, ColorMapAlpha)),
      make_pair(0.85, QColor(187, 238, 108, ColorMapAlpha)),
      make_pair(0.90, QColor(219, 247, 102, ColorMapAlpha)),
      make_pair(0.95, QColor(253, 255,  99, ColorMapAlpha))
    };

    const QMap<double, QColor> ColorMap_PuCy = QMap<double, QColor>{
      make_pair(0.00, QColor( 37,  24,  37, ColorMapAlpha)),
      make_pair(0.10, QColor( 51,  43,  63, ColorMapAlpha)),
      make_pair(0.20, QColor( 59,  65,  90, ColorMapAlpha)),
      make_pair(0.30, QColor( 61,  90, 117, ColorMapAlpha)),
      make_pair(0.40, QColor( 55, 116, 142, ColorMapAlpha)),
      make_pair(0.50, QColor( 39, 143, 162, ColorMapAlpha)),
      make_pair(0.60, QColor(  9, 171, 177, ColorMapAlpha)),
      make_pair(0.70, QColor(  0, 200, 186, ColorMapAlpha)),
      make_pair(0.80, QColor( 48, 228, 188, ColorMapAlpha)),
      make_pair(0.90, QColor( 96, 255, 184, ColorMapAlpha))
    };
  }


  namespace LinearProgramming
  {
    //NOTE: Increase this if you want to solve larger-scaled programs.
    //NOTE: The average number of iterations is 2M + N.
    //NOTE: The maximal number of iterations is Binomial[N + M, N]
    constexpr uint32_t MaxSimplexIterations = 601080390;
  }


  namespace MathUtils
  {
#ifdef LP_WITH_MULTIPRECISION
    const NumericTypes::BoostReal Epsilon = 1E-8; //TODO: ~
    const NumericTypes::BoostInteger DefaultRationalizeDemoninator = 10;
#else
    constexpr NumericTypes::BuiltinReal Epsilon = 1E-8;
    constexpr NumericTypes::BuiltinInteger DefaultRationalizeDemoninator = 10;
#endif // LP_WITH_MULTIPRECISION

    constexpr uint16_t MaxRationalizeIterations = 22;
  }


  namespace TableModelUtils
  {
    constexpr int Seed = 1337;
    constexpr double DistMin = -10.;
    constexpr double DistMax = 10.;
  }
}


#endif // CONFIG_HXX
