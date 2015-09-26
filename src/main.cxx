#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QScopedPointer>
#include <QString>
#include <QStringList>
#include <QStyleFactory>

#include "mainwindow.hxx"

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
8. Работа с обыкновенными и десятичными дробями.
~9. Справка.
~10. Контекстно-зависимая помощь.
+11. Поддержка мыши.
+12. Контроль данных (защита от «дурака»).

Бонусы:
~1. Графический двумерный метод решения.
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

QApplication* create(int argc, char** argv)
{
  QStringList args;
  for (int i = 0; i < argc; ++i)
  {
    args.append(QString(argv[i]));
  }
  QCommandLineParser parser;
  QCommandLineOption styleOption(
    QStringLiteral("style"),
    QStringLiteral("Determines the style of the GUI application."),
    QStringLiteral("style")
  );
  parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
  parser.setApplicationDescription(QStringLiteral("Linear Programming Tool"));
  parser.addVersionOption();
  parser.addHelpOption();
  parser.addOption(styleOption);
  QApplication* app = new QApplication(argc, argv);
  QCoreApplication::setApplicationVersion(QStringLiteral("0.0.1"));
  QCoreApplication::setApplicationName(QStringLiteral("Linear Programming"));
  parser.process(args);
  if (!parser.isSet(styleOption))
  {
    qDebug() << "create: forcing \"style\" value to \"Fusion\"";
    if (QStyleFactory::keys().contains(QStringLiteral("Fusion"), Qt::CaseInsensitive)) {
      QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    }
  }

  return app;
}

int main(int argc, char** argv)
{
//  QApplication::setDesktopSettingsAware(false);
//  QApplication app(argc, argv);
//  auto args = QCoreApplication::arguments();
//  if (args.contains(QStringLiteral("-style"), Qt::CaseInsensitive))
//  {
//    if (QStyleFactory::keys().contains("Fusion", Qt::CaseInsensitive)) {
//       QApplication::setStyle(QStyleFactory::create("Fusion"));
//    }
//  }
//  MainWindow wnd;
//  wnd.show();
//  return app.exec();

  QApplication::setDesktopSettingsAware(false);
  QScopedPointer<QApplication> app(create(argc, argv));
  MainWindow wnd;
  wnd.show();

  return app->exec();
}
