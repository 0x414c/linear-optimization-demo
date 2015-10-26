#include "mainwindow.hxx"
#include "ui_mainwindow.h"

#include <cstddef>
#include <cmath>
#include <iostream>
#include <limits>

#include <QBrush>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QLineF>
#include <QMessageBox>
#include <QPen>
#include <QPointF>
#include <QStandardPaths>
#include <QString>
#include <QVariant>
#include <QVector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Dense"
#include "qcustomplot/qcustomplot.h"

#include "numericstyleditemdelegate.hxx"
#include "tablemodel.hxx"
#include "tablemodelstorage.hxx"
#include "tablemodelutils.hxx"
#include "../lp/graphicalsolver2d.hxx"
#include "../lp/plotdata2d.hxx"
#include "../misc/dataconvertors.hxx"
#include "../math/mathutils.hxx"
#include "../misc/utils.hxx"

using namespace boost;
using namespace Eigen;
using namespace MathUtils;
using namespace std;
using namespace TableModelUtils;
using namespace Utils;

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  _initializeWindowContents();
}

MainWindow::~MainWindow()
{
  _deleteWindowContents();
  delete ui;
}

void MainWindow::_initializeWindowContents()
{
  _ratNumericDelegates = QVector<NumericStyledItemDelegate<rational_t>*>(_modelsCount);
  _realNumericDelegates = QVector<NumericStyledItemDelegate<real_t>*>(_modelsCount);
  _models = QVector<TableModel*>(_modelsCount);
  for (int i = 0; i < _modelsCount; ++i)
  {
    _realNumericDelegates[i] = new NumericStyledItemDelegate<real_t>(this);
    _ratNumericDelegates[i] = new NumericStyledItemDelegate<rational_t>(this);
    _models[i] = new TableModel(this);
  }

  _models[int(Model::ObjFunc)]->insertRow(0);
  _models[int(Model::ObjFunc)]->insertColumns(0, 2);
  ui->objFuncCoeffsTableView->setModel(_models[int(Model::ObjFunc)]);

  _models[int(Model::Constrs)]->insertRows(0, 2);
  _models[int(Model::Constrs)]->insertColumns(0, 2);
  ui->constrsCoeffsTableView->setModel(_models[int(Model::Constrs)]);

  _models[int(Model::RHS)]->insertRows(0, 2);
  _models[int(Model::RHS)]->insertColumn(0);
  ui->constrsRHSTableView->setModel(_models[int(Model::RHS)]);

  _clearModelsContents();
  _setTablesHeaders();
  _updateTableViewsDelegates();
}

void MainWindow::_deleteWindowContents()
{
  for (int i = 0; i < _modelsCount; ++i)
  {
    delete _realNumericDelegates[i];
    delete _ratNumericDelegates[i];
    delete _models[i];
  }
}

void MainWindow::_clearModelsContents()
{
  for (int i = 0; i < _modelsCount; ++i)
  {
    _models[i]->clear(QStringLiteral("0"));
  }
}

void MainWindow::_setTablesHeaders()
{
  _models[int(Model::ObjFunc)]->setHeaderData(0, Qt::Vertical, QStringLiteral("C"));
  _models[int(Model::RHS)]->setHeaderData(0, Qt::Horizontal, QStringLiteral("B"));
}

void MainWindow::_updateTableViewsDelegates()
{
  switch (_field)
  {
    case Field::Real:
      qDebug() << "MainWindow::_updateTableViewsDelegates: switching mode to \"Real\"";
        ui->objFuncCoeffsTableView->setItemDelegate(_realNumericDelegates[int(Model::ObjFunc)]);
        ui->constrsCoeffsTableView->setItemDelegate(_realNumericDelegates[int(Model::Constrs)]);
        ui->constrsRHSTableView->setItemDelegate(_realNumericDelegates[int(Model::RHS)]);
      break;
    case Field::Rational:
      qDebug() << "MainWindow::_updateTableViewsDelegates: switching mode to \"Rational\"";
        ui->objFuncCoeffsTableView->setItemDelegate(_ratNumericDelegates[int(Model::ObjFunc)]);
        ui->constrsCoeffsTableView->setItemDelegate(_ratNumericDelegates[int(Model::Constrs)]);
        ui->constrsRHSTableView->setItemDelegate(_ratNumericDelegates[int(Model::RHS)]);
      break;
    default:
      qDebug() << "MainWindow::_updateTableViewsDelegates: unknown mode";
      break;
  }
}

DialogResult MainWindow::_loadData(const QString& fileName)
{
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning("MainWindow::_loadData: couldn't open file");

      return DialogResult::Fail;
    }
    else
    {
      const QByteArray byteArray = file.readAll();
      const QJsonDocument jsonDocument(QJsonDocument::fromJson(byteArray));
      TableModelStorage tableModelStorage;
      tableModelStorage.read(jsonDocument.object());
      file.flush();
      file.close();

      if (tableModelStorage.length() == _modelsCount)
      {
        for (int i = 0; i < _modelsCount; ++i)
        {
          *_models[i] = tableModelStorage.valueAt(i);
        }
        _setTablesHeaders();

        ui->constrsSpinBox->setValue(tableModelStorage.valueAt(int(Model::Constrs)).rowCount());
        ui->coeffsSpinBox->setValue(tableModelStorage.valueAt(int(Model::Constrs)).columnCount());

        _field = tableModelStorage.field();
        switch (_field)
        {
          case Field::Real:
            ui->realRadioButton->setChecked(true);
            break;
          case Field::Rational:
            ui->rationalRadioButton->setChecked(true);
            break;
          default:
            qDebug() << "MainWindow::_loadData: unknown mode";
            break;
        }

        return DialogResult::Success;
      }
      else
      {
        return DialogResult::Fail;
      }
    }
  }
  else
  {
    return DialogResult::Nothing;
  }
}

DialogResult MainWindow::_saveData(const QString& fileName)
{
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
      qWarning("MainWindow::_saveData: couldn't save file.");

      return DialogResult::Fail;
    }
    else
    {
      QJsonObject jsonObject;
      const QVector<TableModel> models{*_models[int(Model::ObjFunc)], *_models[int(Model::Constrs)], *_models[int(Model::RHS)]}; //TODO: ~
      TableModelStorage tableModelStorage(models, _field);
      tableModelStorage.write(jsonObject);
      const QJsonDocument jsonDocument(jsonObject);
      file.write(jsonDocument.toJson());
      file.flush();
      file.close();

      return DialogResult::Success;
    }
  }
  else
  {
    return DialogResult::Nothing;
  }
}

void MainWindow::on_customPlot_selectionChangedByUser()
{
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
      ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
      ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)
  )
  {
    ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
    ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
  }

  if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
      ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
      ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)
  )
  {
    ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
    ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
  }

  for (int i = 0; i < ui->customPlot->plottableCount(); ++i)
  {
    QCPAbstractPlottable* const plottable = ui->customPlot->plottable(i);
    QCPPlottableLegendItem* const item = ui->customPlot->legend->itemWithPlottable(plottable);
    if (item->selected() || plottable->selected())
    {
      item->setSelected(true);
      plottable->setSelected(true);
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
  int oldRowsCount = _models[int(Model::Constrs)]->rowCount();
  int colsCount = _models[int(Model::Constrs)]->columnCount();

  _models[int(Model::Constrs)]->resize(arg1, 0);
  _models[int(Model::Constrs)]->clear(oldRowsCount, 0, arg1, colsCount, QStringLiteral("0"));

  _models[int(Model::RHS)]->resize(arg1, 0);
  _models[int(Model::RHS)]->clear(oldRowsCount, 0, arg1, 1, QStringLiteral("0"));
}

void MainWindow::on_coeffsSpinBox_valueChanged(int arg1)
{
    int oldColumnsCount = _models[int(Model::Constrs)]->columnCount();
    int rowsCount = _models[int(Model::Constrs)]->rowCount();

  _models[int(Model::Constrs)]->resize(0, arg1);
  _models[int(Model::Constrs)]->clear(0, oldColumnsCount, rowsCount, arg1, QStringLiteral("0"));

  _models[int(Model::ObjFunc)]->resize(0, arg1);
  _models[int(Model::ObjFunc)]->clear(0, oldColumnsCount, 1, arg1, QStringLiteral("0"));
}

void MainWindow::on_realRadioButton_toggled(bool checked)
{
  if (checked)
  {
    _field = Field::Real;
    for (int i = 0; i < _modelsCount; ++i)
    {
      convertTableModel<real_t, rational_t>(_models[i]);
    }
    _updateTableViewsDelegates();
  }
}

void MainWindow::on_rationalRadioButton_toggled(bool checked)
{
  if (checked)
  {
    _field = Field::Rational;
    for (int i = 0; i < _modelsCount; ++i)
    {
      convertTableModel<rational_t, real_t>(_models[i]);
    }
    _updateTableViewsDelegates();
  }
}

void MainWindow::on_action_Open_triggered()
{
  QString fileName = QFileDialog::getOpenFileName(
    this,
    QStringLiteral("Open Data File"),
    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/data.json",
    QStringLiteral("JSON files (*.json);;Text files (*.txt)"),
    0,
    QFileDialog::DontUseNativeDialog
  );

  auto result = _loadData(fileName);
  switch (result)
  {
    case DialogResult::Success:
      setWindowFilePath(fileName);
      setWindowModified(true); //TODO: ~
      break;
    case DialogResult::Fail:
      QMessageBox::critical(
        this,
        "File Reading Error",
        "Could not open selected file."
      );
      break;
    default:
    case DialogResult::Nothing:
      break;
  }
}

void MainWindow::on_action_Save_as_triggered()
{
  QString fileName = QFileDialog::getSaveFileName(
    this,
    QStringLiteral("Save Data File As..."),
    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
    QString("/data@%1.json").arg(QDateTime::currentDateTime().toString(QStringLiteral("dMMMyy_h-m-s"))),
    QStringLiteral("JSON files (*.json);;Text files (*.txt)"),
    0,
    QFileDialog::DontUseNativeDialog
  );

  auto result = _saveData(fileName);
  switch (result)
  {
    case DialogResult::Success:
      setWindowModified(false);
      break;
    case DialogResult::Fail:
      QMessageBox::critical(
        this,
        "File Writing Error",
        "Could not save file to the selected location."
      );
    case DialogResult::Nothing:
    default:
      break;
  }
}

void MainWindow::on_action_Quit_triggered()
{
  QApplication::quit();
}

void MainWindow::on_action_Undo_triggered()
{
  //TODO: ~
}

void MainWindow::on_action_Redo_triggered()
{
  //TODO: ~
}

void MainWindow::on_action_Clear_triggered()
{
  _clearModelsContents();
}

void MainWindow::on_action_Fill_w_random_numbers_triggered()
{
  TableModelUtils::fillTableModel(_models[int(Model::ObjFunc)], TableModelUtils::FillMethod::Random);
  TableModelUtils::fillTableModel(_models[int(Model::Constrs)], TableModelUtils::FillMethod::Random);
  TableModelUtils::fillTableModel(_models[int(Model::RHS)], TableModelUtils::FillMethod::Random);
}

void MainWindow::on_action_Zoom_in_triggered()
{
  //TODO: ~
}

void MainWindow::on_action_Zoom_out_triggered()
{
  //TODO: ~
}

void MainWindow::on_action_Zoom_reset_triggered()
{
  //TODO: ~
}

void MainWindow::on_action_How_to_triggered()
{
  //TODO: ~
}

void MainWindow::on_action_About_triggered()
{
  QMessageBox::about(
    this,
    QStringLiteral("About Application..."),
    QStringLiteral(
      "<b>Linear Optimization v. 0.0.1</b>"
      "<br><br>"
      "Copyright (c) 2015 Alexey Gorishny (<a href=\"http://www.0x414c.tk\">Homepage</a>)"
      "<br><br>"
      "Application for demonstrating some methods of solving linear optimization problems."
      "<br><br>"
      "This software uses:"
      "<ul>"
      "<li><a href=\"http://www.qt.io\">Qt (v. 5.5.0)</a></li>"
      "<li><a href=\"http://eigen.tuxfamily.org\">Eigen (v. 3.2.6)</a></li>"
      "<li><a href=\"http://www.qcustomplot.com\">QCustomPlot (v. 1.3.1)</a></li>"
      "<li><a href=\"http://www.boost.org\">Boost (v. 1.59.0)</a></li>"
      "<li><a href=\"http://cppformat.github.io\">C++ Format (v. 1.1.0)</a></li>"
      "<li><a href=\"http://git.gnome.org/browse/adwaita-icon-theme\">Adwaita Icon Theme (v. 3.16.2.1)</a> by the <a href=\"http://www.gnome.org\">GNOME Project</a></li>"
      //http://www.gnome.org
      //http://wiki.gnome.org/Design
      //http://git.gnome.org/browse/adwaita-icon-theme
      //http://ftp.gnome.org/pub/gnome/sources/adwaita-icon-theme/3.16
      //http://github.com/GNOME/adwaita-icon-theme'
      "</ul>"
      "<br><br>"
      "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR " //TODO: ~
      "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
      "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
      "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
      "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
      "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN "
      "THE SOFTWARE."
    )
  );
}

void MainWindow::on_action_About_Qt_triggered()
{
  QApplication::aboutQt();
}

void MainWindow::on_clearPushButton_clicked()
{
  _clearModelsContents();
}

void MainWindow::on_plotPushButton_clicked()
{
  switch (_field)
  {
    case Field::Real:
      {
        Matrix<real_t, 1, Dynamic> objFuncCoeffs = getRowVector<real_t>(_models[int(Model::ObjFunc)]);
        Matrix<real_t, Dynamic, Dynamic> constrsCoeffs = getMatrix<real_t>(_models[int(Model::Constrs)]);
        Matrix<real_t, Dynamic, 1> constrsRHS = getColumnVector<real_t>(_models[int(Model::RHS)]);

        LinearProgramData<real_t> linearProgramData(objFuncCoeffs, constrsCoeffs, constrsRHS);
        GraphicalSolver2D<real_t> graphicalSolver2D(linearProgramData);
        optional<PlotData2D> plotData2D = graphicalSolver2D.solve();
        if (plotData2D)
        {
          _plotGraph(*plotData2D);
        }
        else
        {
          QMessageBox::warning(
            this,
            QStringLiteral("Oops..."),
            QStringLiteral("This linear optimization problem can not be solved by 2D graphical method.")
          );
        }
        break;
      }
    case Field::Rational:
      break;
    default:
      break;
  }
}

void MainWindow::on_startPushButton_clicked()
{
  //TODO: !
  _runSolver();
}

[[deprecated("To be removed in release version!")]]
void MainWindow::on_testPushButton_clicked()
{
//  _loadData(QStringLiteral("data4.json"));
  _loadData(QStringLiteral("data3+.json"));

  real_t pi =   3.141'592'653'589'793'238'46;
  real_t sq_2 = 1.414'213'562'373'095'048'80;
  real_t e =    2.718'281'828'459'045'235'36;
  for (int exp = 0; exp < 17; ++exp)
  {
    real_t tol = pow(.1, exp);
    auto rat_1 = rationalize<int32_t>(sq_2, tol);
    cerr << exp << "=>" << tol << "==>" << rat_1.first << "/" << rat_1.second << ";\t\t" <<
            endl << std::flush;
  }
}

void MainWindow::_runSolver() {
  _realSolver->solve(); //TODO: !
}

void MainWindow::_setupCustomPlot(QCustomPlot* const customPlot)
{
  customPlot->clearFocus();
  customPlot->clearGraphs();
  customPlot->clearItems();
  customPlot->clearMask();
  customPlot->clearPlottables();
//  customPlot->plotLayout()->removeAt(0);

  //set up all the things
  const QFont normalFont(font().family(), 9);
  const QFont boldFont(font().family(), 9, QFont::Bold);

  customPlot->setInteractions(
    QCP::iRangeDrag | QCP::iRangeZoom |
    QCP::iSelectPlottables | QCP::iSelectAxes |
    QCP::iSelectLegend | QCP::iSelectItems
  );

  customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
//  customPlot->plotLayout()->insertRow(0);
//  customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(customPlot, "Plot (1)."));
  customPlot->legend->setVisible(true);
  customPlot->legend->setFont(normalFont);
  customPlot->legend->setSelectedFont(boldFont);
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
  customPlot->xAxis->setLabel(QString("x%1").arg(1)); //TODO: !
  customPlot->yAxis->setLabel(QString("x%1").arg(2)); //TODO: !

  connect(customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(on_customPlot_selectionChangedByUser()));
  connect(customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(on_customPlot_mousePress()));
  connect(customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(on_customPlot_mouseWheel()));
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

[[deprecated("Needs refactoring.")]]
void MainWindow::_plotGraph(const PlotData2D& plotData2D) {
  auto const customPlot = ui->customPlot;
  _setupCustomPlot(customPlot);

  const QVector<QColor> plotColors{Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow};
  const int colorsCount = plotColors.length();
  const QFont normalFont(font().family(), 9);
  const QFont boldFont(font().family(), 9, QFont::Bold);
  const int verticesCount = plotData2D.vertices.length();

  //Add heatmap for obj. func. values
//  QCPColorMap* colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
//  int nx = 2;
//  int ny = 2;
//  colorMap->data()->setSize(nx, ny);
//  colorMap->data()->setRange(QCPRange(0, 4), QCPRange(0, 3));
//  double x, y, z;
//  for (int xIdx = 0; xIdx < nx; ++xIdx)
//  {
//    for (int yIdx = 0; yIdx <ny; ++yIdx)
//    {
//      colorMap->data()->cellToCoord(xIdx, yIdx, &x, &y);
//      z = -xIdx * yIdx;
//      colorMap->data()->setCell(xIdx, yIdx, z);
//    }
//  }
//  colorMap->setName(QString("f(x%1, x%2)").arg(1).arg(2));
//  colorMap->setTightBoundary(true);
//  colorMap->setGradient(QCPColorGradient::gpHot);
//  colorMap->rescaleDataRange(true);
//  colorMap->updateLegendIcon();
//  customPlot->addPlottable(colorMap);

  //Add non-negativity constraints
  QCPItemStraightLine* const xAxisPlotLine = new QCPItemStraightLine(customPlot);
  xAxisPlotLine->setPen(QPen(Qt::gray));
  xAxisPlotLine->point1->setCoords(QPointF(0, 0));
  xAxisPlotLine->point2->setCoords(QPointF(1, 0));
  xAxisPlotLine->setSelectable(false);
  customPlot->addItem(xAxisPlotLine);

  QCPItemStraightLine* const yAxisPlotLine = new QCPItemStraightLine(customPlot);
  yAxisPlotLine->setPen(QPen(Qt::gray));
  yAxisPlotLine->point1->setCoords(QPointF(0, 0));
  yAxisPlotLine->point2->setCoords(QPointF(0, 1));
  yAxisPlotLine->setSelectable(false);
  customPlot->addItem(yAxisPlotLine);

  //Add feasible region
  QCPCurve* const feasibleRegionCurve = new QCPCurve(customPlot->xAxis, customPlot->yAxis);
  QVector<real_t> x1(verticesCount + 1), y1(verticesCount + 1);
  for (int i = 0; i < verticesCount; ++i)
  {
    x1[i] = plotData2D.vertices[i].x();
    y1[i] = plotData2D.vertices[i].y();
  }
  x1[verticesCount] = plotData2D.vertices[0].x();
  y1[verticesCount] = plotData2D.vertices[0].y();
  feasibleRegionCurve->setData(x1, y1);
  feasibleRegionCurve->setPen(QPen(Qt::transparent));
  feasibleRegionCurve->setBrush(QBrush(QColor(127, 0, 127, 31)));
  feasibleRegionCurve->setSelectedPen(QPen(Qt::transparent));
  feasibleRegionCurve->setSelectedBrush(QBrush(QColor(127, 0, 127, 63)));
  feasibleRegionCurve->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(127, 0, 127), 8));
  feasibleRegionCurve->setName(QStringLiteral("U"));
  feasibleRegionCurve->setSelectable(true);
  customPlot->addPlottable(feasibleRegionCurve);

  //Add constraints
  for (int i = 0; i < verticesCount; ++i) {
    //Constraint as infinite line
    QCPItemStraightLine* const constraintAsContiniousLine = new QCPItemStraightLine(customPlot);
    constraintAsContiniousLine->setPen(QPen(Qt::gray));
    constraintAsContiniousLine->point1->setCoords(plotData2D.vertices[i]);
    constraintAsContiniousLine->point2->setCoords(plotData2D.vertices[(i + 1) % verticesCount]);
    constraintAsContiniousLine->setSelectable(false);
    customPlot->addItem(constraintAsContiniousLine);

    //Constraint as feasible region edge
    QVector<real_t> x(2), y(2); //add edge data
    x[0] = plotData2D.vertices[i].x();
    x[1] = plotData2D.vertices[(i + 1) % verticesCount].x();
    y[0] = plotData2D.vertices[i].y();
    y[1] = plotData2D.vertices[(i + 1) % verticesCount].y();
    customPlot->addGraph(); //create graph and assign data to it
    customPlot->graph(i)->setData(x, y);
    auto defaultColor = plotColors[i % colorsCount]; //set graph look
//    auto brushColor = defaultColor; brushColor.setAlphaF(.25);
//    auto selectedBrushColor = brushColor; selectedBrushColor.setAlphaF(.125);
    customPlot->graph(i)->setPen(QPen(defaultColor));
//    customPlot->graph(i)->setBrush(QBrush(brushColor));
    customPlot->graph(i)->setSelectedPen(QPen(QBrush(defaultColor), 2.5));
//    customPlot->graph(i)->setSelectedBrush(QBrush(selectedBrushColor));
    customPlot->graph(i)->setName(QString("Eq. (%1)").arg(i + 1));
    customPlot->graph(i)->setSelectable(true);
  }

  //Add objective function level line...
  const QLineF levelLine(plotData2D.gradient.normalVector());
  QCPItemStraightLine* const objFuncPlotLine = new QCPItemStraightLine(customPlot);
  objFuncPlotLine->setPen(QPen(Qt::darkBlue));
  objFuncPlotLine->point1->setCoords(levelLine.p1());
  objFuncPlotLine->point2->setCoords(levelLine.p2());
  objFuncPlotLine->setSelectable(true);
  customPlot->addItem(objFuncPlotLine);

  //HACK: Invisible level line (only a section) to obtain anchor for text label
  QCPItemLine* const levelLineSection = new QCPItemLine(customPlot);
  levelLineSection->setPen(QPen(Qt::transparent));
  levelLineSection->start->setCoords(levelLine.p1());
  levelLineSection->end->setCoords(levelLine.p2());
  levelLineSection->setSelectable(false);
  customPlot->addItem(levelLineSection);

  //...w/ label
  QCPItemText* const objFuncLevelLineText = new QCPItemText(customPlot);
  objFuncLevelLineText->setPositionAlignment(Qt::AlignTop | Qt::AlignRight);
  objFuncLevelLineText->position->setParentAnchor(levelLineSection->start);
//  objFuncLevelLineText->position->setType(QCPItemPosition::ptPlotCoords);
  objFuncLevelLineText->position->setCoords(-1., 0.);
  objFuncLevelLineText->setRotation(levelLine.angle());
  objFuncLevelLineText->setText(QStringLiteral("F(⃗x)=const"));
  objFuncLevelLineText->setFont(boldFont);
  objFuncLevelLineText->setSelectable(false);
  customPlot->addItem(objFuncLevelLineText);

  //Ddd gradient vector...
  QCPItemLine* const objFuncGradientVectorArrow = new QCPItemLine(customPlot);
  objFuncGradientVectorArrow->setPen(QPen(Qt::darkRed));
  objFuncGradientVectorArrow->start->setCoords(plotData2D.gradient.p1());
  objFuncGradientVectorArrow->end->setCoords(plotData2D.gradient.p2());
  objFuncGradientVectorArrow->setHead(QCPLineEnding::esLineArrow);
  objFuncGradientVectorArrow->setSelectable(true);
  customPlot->addItem(objFuncGradientVectorArrow);

  //...w/ label
  QCPItemText* const objFuncGradientVectorArrowText = new QCPItemText(customPlot);
  Qt::AlignmentFlag vertAlignment = plotData2D.gradient.p2().y() >= 0. ? Qt::AlignBottom : Qt::AlignTop;
  objFuncGradientVectorArrowText->setPositionAlignment(vertAlignment | Qt::AlignHCenter);
  objFuncGradientVectorArrowText->position->setParentAnchor(objFuncGradientVectorArrow->end);
//  objFuncGradientVectorArrowText->position->setType(QCPItemPosition::ptPlotCoords);
  objFuncGradientVectorArrowText->setFont(boldFont);
  objFuncGradientVectorArrowText->setText(
    QString("∇F(⃗x)=(%1;%2)")
      .arg(plotData2D.gradient.p2().x() - plotData2D.gradient.p1().x())
      .arg(plotData2D.gradient.p2().y() - plotData2D.gradient.p1().y())
  );
  objFuncGradientVectorArrowText->setSelectable(false);
  customPlot->addItem(objFuncGradientVectorArrowText);

//  QCPItemTracer* const phaseTracer = new QCPItemTracer(customPlot);
//  phaseTracer->setGraph(customPlot->graph(1));
//  phaseTracer->setGraphKey(0.);
//  phaseTracer->setInterpolating(true);
//  phaseTracer->setStyle(QCPItemTracer::tsCircle);
//  phaseTracer->setPen(QPen(Qt::red));
//  phaseTracer->setBrush(Qt::red);
//  phaseTracer->setSize(7);
//  customPlot->addItem(phaseTracer);

  //Finalize
  customPlot->rescaleAxes();
  customPlot->replot();
}
