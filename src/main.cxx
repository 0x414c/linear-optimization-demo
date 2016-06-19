#ifdef LP_TEST_MODE
#include "test/test.hxx"
#else // LP_TEST_MODE
#include <clocale>

#include <QApplication>
#include <QLocale>
#include <QScopedPointer>

#include "gui/mainwindow.hxx"
#include "misc/utils.hxx"
#include "config.hxx"
#endif // LP_TEST_MODE


/**
 * @brief main
 * The entry point of the program.
 * @param argc Command-line arguments count.
 * @param argv Command-line arguments values.
 * @return Exit code.
 */
int
main(int argc, char** argv)
{
#ifdef LP_TEST_MODE
//  Test::testSolvers();
//  Test::testUtilities();
  Test::testRREF();
#else // LP_TEST_MODE
  using namespace Config::AppGlobal;

  std::setlocale(LC_ALL, LocaleIdentifier);
  QLocale::setDefault(Locale);
  QApplication::setDesktopSettingsAware(DesktopSettingsAware);

  const QScopedPointer<QApplication> application(
    Utils::makeApplication(argc, argv)
  );

  GUI::MainWindow mainWindow;
  mainWindow.show();

  return application->exec();
#endif
}
