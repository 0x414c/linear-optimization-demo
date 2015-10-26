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

/*
Лабораторная работа по методам оптимизации
Задача: написать программу, позволяющую решать задачу линейного программирования графическим и симплекс- методами.

Основные требования:
+1. Возможность диалогового ввода размерности задачи и матрицы коэффициентов целевой функции в канонической форме. Размерность не более 16*16.
+2. Сохранение введенной задачи в файл и чтение из файла.
3. Возможность решения задачи с использованием заданных базисных переменных.
4. Реализация метода искусственного базиса.
5. Выбор автоматического и пошагового режима решения задачи.
6. В пошаговом режиме возможность возврата назад.
7. В пошаговом режиме возможность выбора опорного элемента.
+8. Работа с обыкновенными и десятичными дробями.
9. Справка.
10. Контекстно-зависимая помощь.
+11. Поддержка мыши.
+12. Контроль данных (защита от «дурака»).

Бонусы:
+1. Графический двумерный метод решения.
2. Графический трехмерный метод решения.
3. Графический метод решения целочисленной задачи.
4. Решение двойственной задачи.
5. Метод Гомори решения целочисленной задачи.

Программа должна быть оконным приложением. Сдается выполняемый eхe-файл, все файлы проекта и откомментированные исходники.
Нa чём писать:
+1. C++
2. C#
3. Java
4. ...
*/


/*
Code style rules:
1. Include order:
  "This class header" (optional)
  <C++ headers>
  <C headers>
  <Qt headers>
  "Foreign library headers"
  "Local submodule headers"
  "Foreign submodules headers"
2. Indent size: 2 spaces, Tab size: 4 spaces.
3. Templates:
  Declarations in *.hxx should be followed by inclusion of *.txx
  file w/ corresponding definitions and/or specializations.
*/


/**
 * @brief createApplication
 * This QApplication factory can override Qt options.
 * It allows user to set style as usual by using command-line args,
 * but will stick to Qt 5' `Fusion' style by default.
 * @param argc
 * @param argv
 * @return
 */
QApplication* createApplication(int& argc, char** argv)
{
  QStringList args;
  for (int i = 0; i < argc; ++i)
  {
    args.append(QString(argv[i]));
  }

  QApplication* const app = new QApplication(argc, argv);
  QCoreApplication::setApplicationName(QStringLiteral("Linear Optimization"));
  QCoreApplication::setOrganizationName(QStringLiteral("0x414c!"));
  QCoreApplication::setOrganizationDomain(QStringLiteral("www.0x414c.tk"));
  QCoreApplication::setApplicationVersion(QStringLiteral("0.0.1"));
  QApplication::setApplicationDisplayName(QStringLiteral("Linear Optimization"));

  QCommandLineParser parser;
  const QCommandLineOption styleOption(
    QStringLiteral("style"),
    QStringLiteral("Sets the application GUI style. Possible values depend on your system configuration."),
    QStringLiteral("name")
  );
  const QCommandLineOption stylesheetOption(
    QStringLiteral("stylesheet"),
    QStringLiteral("Sets the application Style Sheet. The value must be a path to a file that contains the Style Sheet."),
    QStringLiteral("path")
  );
  const QCommandLineOption widgetcountOption(
    QStringLiteral("widgetcount"),
    QStringLiteral("Prints debug message at the end about number of widgets left undestroyed and maximum number of widgets existed at the same time.")
  );
  const QCommandLineOption reverseOption(
    QStringLiteral("reverse"),
    QStringLiteral("Sets the application's layout direction to Right-To-Left.")
  );
  const QCommandLineOption qmljsdebuggerOption(
    QStringLiteral("qmljsdebugger"),
    QStringLiteral("Activates the QML/JS debugger with a specified port. The value must be of format port:1234[,block], where block is optional and will make the application wait until a debugger connects to it."),
    QStringLiteral("port")
  );
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  parser.setApplicationDescription(QStringLiteral("Linear Optimization Demonstrational Tool"));
  parser.addVersionOption();
  parser.addHelpOption();
  parser.addOption(styleOption);
  parser.addOption(stylesheetOption);
  parser.addOption(widgetcountOption);
  parser.addOption(reverseOption);
  parser.addOption(qmljsdebuggerOption);
  parser.process(args);
  if (!parser.isSet(styleOption))
  {
    qDebug() << "main::createApplication: setting \"style\" value to \"Fusion\"";
    if (QStyleFactory::keys().contains(QStringLiteral("Fusion"), Qt::CaseInsensitive))
    {
      QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    }
  }

  return app;
}

/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv)
{
  setlocale(LC_ALL, "C");
  QApplication::setDesktopSettingsAware(false);
  const QScopedPointer<QApplication> app(createApplication(argc, argv));
  MainWindow wnd;
  wnd.show();
  const int ret = app->exec();

  return ret;
}
