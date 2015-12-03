#include <clocale>

#include <QApplication>
#include <QScopedPointer>

#include "gui/mainwindow.hxx"
#include "misc/utils.hxx"
#include "test/test.hxx"


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
//  setlocale(LC_ALL, "C");
//  QApplication::setDesktopSettingsAware(false);

//  const QScopedPointer<QApplication> application(
//    Utils::makeApplication(argc, argv)
//  );

//  GUI::MainWindow mainWindow;
//  mainWindow.show();

//  return application->exec();

  Test::run();
}
