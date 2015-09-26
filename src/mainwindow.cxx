#include "mainwindow.hxx"
#include "ui_mainwindow.h"

#include <iostream>

#include <QBrush>
#include <QDataStream>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QLineF>
#include <QList>
#include <QMessageBox>
#include <QPen>
#include <QPointF>
#include <QStandardPaths>
#include <QString>
#include <QVector>

#include "lib/qcustomplot/qcustomplot.h"
#include "numericvalidatordelegate.hxx"
#include "problemdataserializable.hxx"
#include "tablewidgetserializable.hxx"
#include "tablewidgetutils.hxx"

using namespace Eigen;
using namespace std;

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  initializeContents();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::initializeContents()
{
  ui->objFuncCoeffTableWidget->setItemDelegate(new NumericValidatorDelegate);
  ui->constrCoeffsTableWidget->setItemDelegate(new NumericValidatorDelegate);
  ui->rhsTableWidget->setItemDelegate(new NumericValidatorDelegate);

  ui->objFuncCoeffTableWidget->setVerticalHeaderLabels(QStringList(QStringLiteral("c")));
  ui->rhsTableWidget->setHorizontalHeaderLabels(QStringList(QStringLiteral("b")));

  clearContents();
}

void MainWindow::clearContents() {
  TableWidgetUtils::clearTable(ui->objFuncCoeffTableWidget);
  TableWidgetUtils::clearTable(ui->constrCoeffsTableWidget);
  TableWidgetUtils::clearTable(ui->rhsTableWidget);
}

Ui::DialogResult MainWindow::load(QString fileName)
{
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning("Couldn't open file.");

      return Ui::DialogResult::Fail;
    }
    else
    {
      QByteArray saveByteArray = file.readAll();
      QJsonDocument dataJsonDocument(QJsonDocument::fromJson(saveByteArray));
      ProblemDataSerializable dataContainer;
      dataContainer.read(dataJsonDocument.object());
      file.flush();
      file.close();

      QVector<QTableWidget*> tableWidgets = {ui->objFuncCoeffTableWidget, ui->constrCoeffsTableWidget, ui->rhsTableWidget};
      TableWidgetUtils::FillTables(tableWidgets, dataContainer);

      ui->constrsSpinBox->setValue(dataContainer.itemAt(1).rows());
      ui->coeffsSpinBox->setValue(dataContainer.itemAt(1).cols());

      return Ui::DialogResult::Success;
    }
  }
  else
  {
    return Ui::DialogResult::Nothing;
  }
}

Ui::DialogResult MainWindow::save(QString fileName)
{
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
      qWarning("Couldn't save file.");

      return Ui::DialogResult::Fail;
    }
    else
    {
      QJsonObject dataJsonObject;
      QVector<QTableWidget*> tableWidgets = {ui->objFuncCoeffTableWidget, ui->constrCoeffsTableWidget, ui->rhsTableWidget};
      ProblemDataSerializable dataContainer(tableWidgets);
      dataContainer.write(dataJsonObject);
      QJsonDocument saveDoc(dataJsonObject);
      file.write(saveDoc.toJson());
      file.flush();
      file.close();

      return Ui::DialogResult::Success;
    }
  }
  else
  {
    return Ui::DialogResult::Nothing;
  }
}

void MainWindow::on_customPlot_selectionChangedByUser()
{
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }

  for (int i = 0; i < ui->customPlot->graphCount(); ++i)
  {
    QCPGraph *graph = ui->customPlot->graph(i);
    QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
    if (item->selected() || graph->selected())
    {
      item->setSelected(true);
      graph->setSelected(true);
    }
  }
}

void MainWindow::on_customPlot_mousePress()
{
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
  }
  else
  {
    if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
      ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
    }
    else
    {
      ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    }
  }
}

void MainWindow::on_customPlot_mouseWheel()
{
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
  }
  else
  {
    if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
      ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
    }
    else
    {
      ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }
  }
}

void MainWindow::on_constrsSpinBox_valueChanged(int arg1)
{
  int oldRowCount = ui->constrCoeffsTableWidget->rowCount();

  ui->constrCoeffsTableWidget->setRowCount(arg1);
  ui->rhsTableWidget->setRowCount(arg1);

  TableWidgetUtils::clearTable(ui->constrCoeffsTableWidget, oldRowCount, 0);
  TableWidgetUtils::clearTable(ui->rhsTableWidget, oldRowCount, 0);
}

void MainWindow::on_coeffsSpinBox_valueChanged(int arg1)
{
  int oldColumnCount = ui->constrCoeffsTableWidget->columnCount();

  ui->constrCoeffsTableWidget->setColumnCount(arg1);
  ui->objFuncCoeffTableWidget->setColumnCount(arg1);

  TableWidgetUtils::clearTable(ui->objFuncCoeffTableWidget, 0, oldColumnCount);
  TableWidgetUtils::clearTable(ui->constrCoeffsTableWidget, 0, oldColumnCount);
}

void MainWindow::on_action_Open_triggered()
{
  QString fileName = QFileDialog::getOpenFileName(
    this,
    QStringLiteral("Open Problem Data File"),
//    QStringLiteral("data.json"),
    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/data.json",
    QStringLiteral("JSON files (*.json);;Text files (*.txt)"),
    0,
    QFileDialog::DontUseNativeDialog
  );

  auto result = load(fileName);
  if (result == Ui::DialogResult::Success)
  {
    setWindowModified(true);
    setWindowFilePath(fileName);
  }
  else
  {
    if (result == Ui::DialogResult::Fail)
    {
      QMessageBox::critical(
        this,
        "File Reading Error",
        "Could not open selected file."
      );
    }
  }
}

void MainWindow::on_action_Save_as_triggered()
{
  QString fileName = QFileDialog::getSaveFileName(
    this,
    QStringLiteral("Open Problem Data File"),
//    QString("data@%1.json").arg(QDateTime::currentDateTime().toString(QStringLiteral("dMMMyy_h-m-s"))),
    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + QString("/data@%1.json").arg(QDateTime::currentDateTime().toString(QStringLiteral("dMMMyy_h-m-s"))),
    QStringLiteral("JSON files (*.json);;Text files (*.txt)"),
    0,
    QFileDialog::DontUseNativeDialog
  );

  auto result = save(fileName);
  if (result == Ui::DialogResult::Success)
  {
    setWindowModified(false);
  }
  else
  {
    if (result == Ui::DialogResult::Fail)
    {
      QMessageBox::critical(
        this,
        "File Writing Error",
        "Could not save file to selected location."
      );
    }
  }
}

void MainWindow::on_action_Quit_triggered()
{
  QApplication::quit();
}

void MainWindow::on_action_Undo_triggered()
{

}

void MainWindow::on_action_Redo_triggered()
{

}

void MainWindow::on_action_Clear_triggered()
{
  clearContents();
}

void MainWindow::on_action_Fill_w_random_numbers_triggered()
{
  TableWidgetUtils::fillTable(ui->objFuncCoeffTableWidget, TableWidgetUtils::Random);
  TableWidgetUtils::fillTable(ui->constrCoeffsTableWidget, TableWidgetUtils::Random);
  TableWidgetUtils::fillTable(ui->rhsTableWidget, TableWidgetUtils::Random);
}

void MainWindow::on_action_Zoom_in_triggered()
{

}

void MainWindow::on_action_Zoom_out_triggered()
{

}


void MainWindow::on_action_Zoom_reset_triggered()
{

}

void MainWindow::on_action_How_to_triggered()
{

}

void MainWindow::on_action_About_triggered()
{

}

void MainWindow::on_action_About_Qt_triggered()
{
  QApplication::aboutQt();
}

void MainWindow::on_clearPushButton_clicked()
{
  clearContents();
}

void MainWindow::on_plotPushButton_clicked()
{
  plot();
}

void MainWindow::on_startPushButton_clicked()
{
  solve();
}

[[deprecated("To be removed in release version.")]]
void MainWindow::on_testPushButton_clicked()
{
  load(QStringLiteral("data4.json"));
}

[[deprecated("Needs refactoring.")]]
void MainWindow::solve() {
  auto A = TableWidgetUtils::getMatrix<double>(ui->constrCoeffsTableWidget);
  auto b = TableWidgetUtils::getMatrix<double>(ui->rhsTableWidget);
  auto c = TableWidgetUtils::getMatrix<double>(ui->objFuncCoeffTableWidget);

  cerr << "c, A, b :=" << endl << c << endl << A << endl << b << endl << endl;

//  FullPivLU<Matrix<double, Dynamic, Dynamic>> lu(A);
//  cerr << "LU(A):=" << endl << lu.matrixLU() << endl;

//  auto x = A.fullPivLu().solve(b);
//  if((A * x).isApprox(b))
//  {
//    cerr << "Here is a solution x to the equation Ax==b:" << endl << x << endl;
//    double relative_error = (A * x - b).norm() / b.norm(); // norm() is L2 norm
//    cout << "The relative error is:\n" << relative_error << endl;
//  }
//  else
//  {
//    cerr << "The equation Ax==b does not have any solution." << endl;
//  }

//  ui->debugTextBrowser->setText("Solving...\n");
}

[[deprecated("Needs refactoring.")]]
void MainWindow::plot() {
  int xAxisIdx = 1;
  int yAxisIdx = 2;
  QVector<QLineF> constrsLines = { QLineF(0, 1, .5, 0), QLineF(0, .5, 1, 0), QLineF(0, .6, .6, 0) };
  int constrsCount = constrsLines.length();
  QLineF objFuncLine(-.25, .5, .5, -.75);
  QLineF objFuncLineNormal = objFuncLine.normalVector();
  qDebug() << objFuncLineNormal;
//  objFuncLineNormal.setLength(1.);

  QVector<QColor> colors = { Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow, Qt::black };

  auto customPlot = ui->customPlot;

  customPlot->clearFocus();
  customPlot->clearGraphs();
  customPlot->clearItems();
  customPlot->clearMask();
  customPlot->clearPlottables();
//  customPlot->plotLayout()->removeAt(0);

  // set up all the things:
  auto plotFont = QFont(font().family(), 8);
  customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables |
                              QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectItems);
  customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
//  customPlot->plotLayout()->insertRow(0);
//  customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(customPlot, "Plot (1)."));
  customPlot->legend->setVisible(true);
  customPlot->legend->setFont(plotFont);
  customPlot->legend->setSelectedFont(plotFont);
  customPlot->legend->setSelectableParts(QCPLegend::spItems);
//  customPlot->axisRect()->setupFullAxesBox(true);
  customPlot->xAxis2->setVisible(true);
  customPlot->xAxis2->setTickLabels(false);
  customPlot->yAxis2->setVisible(true);
  customPlot->yAxis2->setTickLabels(false);
  customPlot->xAxis->setSelectableParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
  customPlot->yAxis->setSelectableParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
  customPlot->xAxis->setRange(0, 2);
  customPlot->yAxis->setRange(0, 1);
  customPlot->xAxis->setLabel(QString("x%1").arg(xAxisIdx));
  customPlot->yAxis->setLabel(QString("x%1").arg(yAxisIdx));
//  connect(customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(on_customPlot_selectionChangedByUser()));
//  connect(customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(on_customPlot_mousePress()));
//  connect(customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(on_customPlot_mouseWheel()));
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // add all constraints plots:
  for (int i = 0; i < constrsCount; ++i) {
    // generate some data:
    QVector<qreal> x(2), y(2);
    x[0] = constrsLines[i].p1().x(); y[0] = constrsLines[i].p1().y();
    x[1] = constrsLines[i].p2().x(); y[1] = constrsLines[i].p2().y();
    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph(i)->setData(x, y);
    // set graph look:
    auto color = colors[i % colors.length()];
    auto color2 = color;
    color2.setAlphaF(.25);
    auto color3 = color2;
    color3.setAlphaF(.125);
    customPlot->graph(i)->setPen(QPen(color));
    customPlot->graph(i)->setBrush(QBrush(color2));
    customPlot->graph(i)->setSelectedBrush(QBrush(color3));
    customPlot->graph(i)->setSelectedPen(QPen(QBrush(color), 2.));
    customPlot->graph(i)->setName(QString("Eq. (%1)").arg(i + 1));
  }

  // add obj. func. plot:
  QCPItemStraightLine *objFuncPlotLine = new QCPItemStraightLine(customPlot);
  customPlot->addItem(objFuncPlotLine);
  objFuncPlotLine->setPen(QPen(Qt::darkBlue));
  objFuncPlotLine->point1->setCoords(objFuncLine.p1());
  objFuncPlotLine->point2->setCoords(objFuncLine.p2());

  // add normal vec. arrow:
  QCPItemLine *objFuncGradientVectorArrow = new QCPItemLine(customPlot);
  customPlot->addItem(objFuncGradientVectorArrow);
  objFuncGradientVectorArrow->setPen(QPen(Qt::darkRed));
  objFuncGradientVectorArrow->start->setCoords(objFuncLineNormal.p1());
  objFuncGradientVectorArrow->end->setCoords(objFuncLineNormal.p2());
  objFuncGradientVectorArrow->setHead(QCPLineEnding::esSpikeArrow);

  // w/ label
  QCPItemText *objFuncGradientVectorArrowText = new QCPItemText(customPlot);
  customPlot->addItem(objFuncGradientVectorArrowText);
  objFuncGradientVectorArrowText->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
  objFuncGradientVectorArrowText->position->setType(QCPItemPosition::ptAxisRectRatio);
  objFuncGradientVectorArrowText->position->setParentAnchor(objFuncGradientVectorArrow->end);
  objFuncGradientVectorArrowText->setText(QString("n=(%1; %2)").arg(objFuncLineNormal.p2().x() - objFuncLineNormal.p1().x()).arg(objFuncLineNormal.p2().y() - objFuncLineNormal.p1().y()));
  objFuncGradientVectorArrowText->setFont(plotFont);
  objFuncGradientVectorArrowText->setSelectable(false);

  // add non-neg. constrs f/ x & y
  QCPItemStraightLine *xConstrPlotLine = new QCPItemStraightLine(customPlot);
  customPlot->addItem(xConstrPlotLine);
  xConstrPlotLine->setPen(QPen(Qt::darkGreen));
  xConstrPlotLine->point1->setCoords(QPointF(0, 0));
  xConstrPlotLine->point2->setCoords(QPointF(1, 0));

  QCPItemStraightLine *yConstrPlotLine = new QCPItemStraightLine(customPlot);
  customPlot->addItem(yConstrPlotLine);
  yConstrPlotLine->setPen(QPen(Qt::darkGreen));
  yConstrPlotLine->point1->setCoords(QPointF(0, 0));
  yConstrPlotLine->point2->setCoords(QPointF(0, 1));

  // add heatmap for obj. func. values
//  QCPColorMap *colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
//  customPlot->addPlottable(colorMap);
//  int nx = 10;
//  int ny = 10;
//  colorMap->data()->setSize(nx, ny);
//  colorMap->data()->setRange(QCPRange(0, 1), QCPRange(0, 1));
//  double x, y, z;
//  for (int xIdx = 0; xIdx < nx; ++xIdx)
//  {
//    for (int yIdx = 0; yIdx <ny; ++yIdx)
//    {
//      colorMap->data()->cellToCoord(xIdx, yIdx, &x, &y);
//      double r = 3*qSqrt(x*x+y*y)+1e-2;
//      z = 2*x*(qCos(r+2)/r-qSin(r+2)/r); // the B field strength of dipole radiation (modulo physical constants)
//      colorMap->data()->setCell(xIdx, yIdx, z);
//    }
//  }
//  colorMap->setName(QString("f(x%1, x%2)").arg(xAxisIdx).arg(yAxisIdx));
//  colorMap->setTightBoundary(true);
//  colorMap->setGradient(QCPColorGradient::gpNight);
//  colorMap->rescaleDataRange(true);
//  colorMap->updateLegendIcon();

  // finalize
  customPlot->rescaleAxes();
  customPlot->replot();
}
