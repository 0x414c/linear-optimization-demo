#ifdef LP_TEST_MODE
#include "test/test.hxx"
#else
#include <clocale>

#include <QApplication>
#include <QScopedPointer>

#include "gui/mainwindow.hxx"
#include "misc/utils.hxx"
#endif // LP_TEST_MODE


/**
 * @brief main
 * The main entry point of the program.
 * @param argc Command-line arguments count.
 * @param argv Command-line arguments values.
 * @return Exit code.
 */
int
main(int argc, char** argv)
{
#ifdef LP_TEST_MODE
  Test::run();
#else
  setlocale(LC_ALL, "C");
  QApplication::setDesktopSettingsAware(false);

  const QScopedPointer<QApplication> application(
    Utils::makeApplication(argc, argv)
  );

  GUI::MainWindow mainWindow;
  mainWindow.show();

  return application->exec();
#endif
}
