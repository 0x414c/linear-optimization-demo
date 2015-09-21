#include <QApplication>
#include <QStyleFactory>
#include <QDebug>
#include "mainwindow.hxx"

int main(int argc, char** argv)
{  
  QApplication app(argc, argv);

  if (QStyleFactory::keys().contains("Fusion", Qt::CaseInsensitive)) {
     QApplication::setStyle(QStyleFactory::create("Fusion"));
  }

  MainWindow wnd;
  wnd.show();

  return app.exec();
}
