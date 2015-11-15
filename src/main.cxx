#include "main.hxx"

#include <clocale>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QStyleFactory>

#include "ui/mainwindow.hxx"

/**
 * @brief makeApplication
 * This `QApplication' factory can override common Qt options.
 * It allows user to set style as usual by using command-line args,
 * but will stick to Qt5's `Fusion' style by default.
 * TODO: ~ Replace this parser w/ dummy options w/ smth cleaner
 * (e.g. just check for presence of "style" in `argv').
 * @param argc Command-line arguments count.
 * @param argv Command-line arguments values.
 * @return Pointer to the QAplication object created using `argc' and `argv'.
 */
QApplication*
makeApplication(int& argc, char** argv)
{
  QStringList args;
  for (int i(0); i < argc; ++i)
  {
    args.append(QString(argv[i]));
  }

  QApplication* const app = new QApplication(argc, argv);
  QCoreApplication::setApplicationName(QStringLiteral("Linear Optimization"));
  QCoreApplication::setOrganizationName(QStringLiteral("0x414c!"));
  QCoreApplication::setOrganizationDomain(QStringLiteral("www.0x414c.tk"));
  QCoreApplication::setApplicationVersion(QStringLiteral("0.0.1"));
  QApplication::setApplicationDisplayName(
    QStringLiteral("Linear Optimization")
  );

  const QCommandLineOption styleOption(
    QStringLiteral("style"),
    QStringLiteral("Sets the application GUI style."
                   " Possible values depend on your system configuration."),
    QStringLiteral("name")
  );
  const QCommandLineOption stylesheetOption(
    QStringLiteral("stylesheet"),
    QStringLiteral("Sets the application Style Sheet."
                   " The value must be a path to a file that contains"
                   " the Style Sheet."),
    QStringLiteral("path")
  );
  const QCommandLineOption widgetcountOption(
    QStringLiteral("widgetcount"),
    QStringLiteral("Prints debug message at the end about number of widgets"
                   " left undestroyed and maximum number of widgets existed"
                   " at the same time.")
  );
  const QCommandLineOption reverseOption(
    QStringLiteral("reverse"),
    QStringLiteral("Sets the application's layout direction to Right-To-Left.")
  );
  const QCommandLineOption qmljsdebuggerOption(
    QStringLiteral("qmljsdebugger"),
    QStringLiteral("Activates the QML/JS debugger with a specified port."
                   " The value must be of format port:1234[,block], where"
                   " block is optional and will make the application wait"
                   " until a debugger connects to it."),
    QStringLiteral("port")
  );
  const QCommandLineOption nograbOption(
    QStringLiteral("nograb"),
    QStringLiteral("The application should never grab the mouse or the"
                   " keyboard."
                   " This option is set by default when the program is"
                   " running in the gdb debugger under Linux.")
    );
  const QCommandLineOption dograbOption(
    QStringLiteral("dograb"),
    QStringLiteral("Ignore any implicit or explicit -nograb."
                   " -dograb wins over -nograb even when -nograb"
                   " is last on the command line.")
  );

  QCommandLineParser parser;
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  parser.setApplicationDescription(
    QStringLiteral("Linear Optimization Demonstrational Tool")
  );
  parser.addVersionOption();
  parser.addHelpOption();
  parser.addOption(styleOption);
  parser.addOption(stylesheetOption);
  parser.addOption(widgetcountOption);
  parser.addOption(reverseOption);
  parser.addOption(qmljsdebuggerOption);
  parser.addOption(nograbOption);
  parser.addOption(dograbOption);
  parser.process(args);
  if (!parser.isSet(styleOption))
  {
    qDebug() << "::makeApplication: setting `style' value to `Fusion'";
    if (
      QStyleFactory::keys().contains(
        QStringLiteral("Fusion"), Qt::CaseInsensitive
      )
    )
    {
      QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    }
  }

  return app;
}

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
  setlocale(LC_ALL, "C");
  QApplication::setDesktopSettingsAware(false);

  const QScopedPointer<QApplication> application(makeApplication(argc, argv));
  MainWindow mainWindow;
  mainWindow.show();

  return application->exec();
}
