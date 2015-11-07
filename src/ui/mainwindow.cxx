#include "mainwindow.hxx"
#include "ui_mainwindow.h"

#include <cstddef>
#include <cmath>
#include <iostream>
#include <limits>
#include <utility>

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
#include "../lp/linearprogramsolution.hxx"
#include "../lp/linearprogramutils.hxx"
#include "../lp/plotdata2d.hxx"
#include "../math/mathutils.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"

using namespace boost;
using namespace Eigen;
using namespace std;
using namespace Utils;

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  initializeWindowContents();
}

MainWindow::~MainWindow()
{
  deleteWindowContents();
  delete ui;
}

void MainWindow::initializeWindowContents()
{
  _ratNumericDelegates = QVector<NumericStyledItemDelegate<Rational>*>(_tableModelsCount);
  _realNumericDelegates = QVector<NumericStyledItemDelegate<Real>*>(_tableModelsCount);
  _tableModels = QVector<TableModel*>(_tableModelsCount);
  for (int i(0); i < _tableModelsCount; ++i)
  {
    _realNumericDelegates[i] = new NumericStyledItemDelegate<Real>(this);
    _ratNumericDelegates[i] = new NumericStyledItemDelegate<Rational>(this);
    _tableModels[i] = new TableModel(this);
  }

  _tableModels[int(ModelType::ObjFunc)]->insertRow(0);
  _tableModels[int(ModelType::ObjFunc)]->insertColumns(0, 2);
  ui->objFuncCoeffsTableView->setModel(_tableModels[int(ModelType::ObjFunc)]);

  _tableModels[int(ModelType::Constrs)]->insertRows(0, 2);
  _tableModels[int(ModelType::Constrs)]->insertColumns(0, 2);
  ui->constrsCoeffsTableView->setModel(_tableModels[int(ModelType::Constrs)]);

  _tableModels[int(ModelType::RHS)]->insertRows(0, 2);
  _tableModels[int(ModelType::RHS)]->insertColumn(0);
  ui->constrsRHSTableView->setModel(_tableModels[int(ModelType::RHS)]);

  clearModelsContents();
  setTablesHeaders();
  updateTableViewsDelegates();
}

void MainWindow::deleteWindowContents()
{
  for (int i(0); i < _tableModelsCount; ++i)
  {
    delete _realNumericDelegates[i];
    delete _ratNumericDelegates[i];
    delete _tableModels[i];
  }
}

void MainWindow::clearModelsContents()
{
  for (int i(0); i < _tableModelsCount; ++i)
  {
    _tableModels[i]->clear(QStringLiteral("0"));
  }
}

void MainWindow::setTablesHeaders()
{
  _tableModels[int(ModelType::ObjFunc)]->setHeaderData(0, Qt::Vertical, QStringLiteral("C"));
  _tableModels[int(ModelType::RHS)]->setHeaderData(0, Qt::Horizontal, QStringLiteral("B"));
}

void MainWindow::updateTableViewsDelegates()
{
  switch (_currentField)
  {
    case Field::Real:
      qDebug() << "MainWindow::_updateTableViewsDelegates: switching mode to \"Real\"";
        ui->objFuncCoeffsTableView->setItemDelegate(_realNumericDelegates[int(ModelType::ObjFunc)]);
        ui->constrsCoeffsTableView->setItemDelegate(_realNumericDelegates[int(ModelType::Constrs)]);
        ui->constrsRHSTableView->setItemDelegate(_realNumericDelegates[int(ModelType::RHS)]);
      break;
    case Field::Rational:
      qDebug() << "MainWindow::_updateTableViewsDelegates: switching mode to \"Rational\"";
        ui->objFuncCoeffsTableView->setItemDelegate(_ratNumericDelegates[int(ModelType::ObjFunc)]);
        ui->constrsCoeffsTableView->setItemDelegate(_ratNumericDelegates[int(ModelType::Constrs)]);
        ui->constrsRHSTableView->setItemDelegate(_ratNumericDelegates[int(ModelType::RHS)]);
      break;
    default:
      qWarning() << "MainWindow::_updateTableViewsDelegates: unknown mode";
      break;
  }
}

OperationResult MainWindow::loadData(const QString& fileName)
{
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning() << QStringLiteral("MainWindow::_loadData: couldn't open file") << fileName;

      return OperationResult::Nothing;
    }
    else
    {
      const QByteArray byteArray(file.readAll());
      if (byteArray.count() > 0)
      {
        const QJsonDocument jsonDocument(QJsonDocument::fromJson(byteArray));
        TableModelStorage tableModelStorage;
        const OperationResult res(tableModelStorage.read(jsonDocument.object()));
        file.flush();
        file.close();

        if (res == OperationResult::Success)
        {
          if (tableModelStorage.count() == _tableModelsCount)
          {
            for (int i(0); i < _tableModelsCount; ++i)
            {
              (*_tableModels[i]) = std::move(tableModelStorage[i]); //Transfer contents...
              (*_tableModels[i]).setParent(this); //...and change ownerhsip
            }
            setTablesHeaders();

            ui->constrsSpinBox->setValue(_tableModels[int(ModelType::Constrs)]->rowCount());
            ui->varsSpinBox->setValue(_tableModels[int(ModelType::Constrs)]->columnCount());

            _currentField = tableModelStorage.field();
            switch (_currentField)
            {
              case Field::Real:
                ui->realRadioButton->setChecked(true);
                break;
              case Field::Rational:
                ui->rationalRadioButton->setChecked(true);
                break;
              default:
                qWarning() << "MainWindow::_loadData: unknown value of `Field'.";
                return OperationResult::Fail;
            }

            return OperationResult::Success;
          }
          else
          {
            return OperationResult::Nothing;
          }
        }
        else
        {
          qWarning() << "MainWindow::_loadData: couldn't correctly parse file contents:"
                        " the file is either incompatible or invalid.";

          return res;
        }
      }
      else
      {
        qWarning() << "MainWindow::_loadData: couldn't read file contents:"
                      " the file is empty.";

        return OperationResult::Nothing;
      }
    }
  }
  else
  {
    return OperationResult::Nothing;
  }
}

OperationResult MainWindow::saveData(const QString& fileName)
{
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
      qWarning() << QStringLiteral("MainWindow::_saveData: couldn't save file to") << fileName;

      return OperationResult::Nothing;
    }
    else
    {
      QJsonObject jsonObject;
      //TODO: ~? Use pointers here
      TableModelStorage tableModelStorage(
        QVector<TableModel>{
          (*_tableModels[int(ModelType::ObjFunc)]),
          (*_tableModels[int(ModelType::Constrs)]),
          (*_tableModels[int(ModelType::RHS)])
        },
        _currentField
      );
      const OperationResult res(tableModelStorage.write(jsonObject));

      if (res == OperationResult::Success)
      {
        const QJsonDocument jsonDocument(jsonObject);

        const qint64 bytesCount(file.write(jsonDocument.toJson(QJsonDocument::Indented)));
        file.flush();
        file.close();
        if (bytesCount == -1)
        {
          return OperationResult::Fail;
        }
        else
        {
          return OperationResult::Success;
        }
      }
      else
      {
        return res;
      }
    }
  }
  else
  {
    return OperationResult::Nothing;
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

  for (int i(0); i < ui->customPlot->plottableCount(); ++i)
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
  int oldRowsCount(_tableModels[int(ModelType::Constrs)]->rowCount());
  int colsCount(_tableModels[int(ModelType::Constrs)]->columnCount());

  _tableModels[int(ModelType::Constrs)]->resize(arg1, 0);
  _tableModels[int(ModelType::Constrs)]->clear(oldRowsCount, 0, arg1, colsCount, QStringLiteral("0"));

  _tableModels[int(ModelType::RHS)]->resize(arg1, 0);
  _tableModels[int(ModelType::RHS)]->clear(oldRowsCount, 0, arg1, 1, QStringLiteral("0"));
}

void MainWindow::on_varsSpinBox_valueChanged(int arg1)
{
  int oldColsCount(_tableModels[int(ModelType::Constrs)]->columnCount());
  int rowsCount(_tableModels[int(ModelType::Constrs)]->rowCount());

  _tableModels[int(ModelType::Constrs)]->resize(0, arg1);
  _tableModels[int(ModelType::Constrs)]->clear(0, oldColsCount, rowsCount, arg1, QStringLiteral("0"));

  _tableModels[int(ModelType::ObjFunc)]->resize(0, arg1);
  _tableModels[int(ModelType::ObjFunc)]->clear(0, oldColsCount, 1, arg1, QStringLiteral("0"));
}

void MainWindow::on_realRadioButton_toggled(bool checked)
{
  if (checked)
  {
    _currentField = Field::Real;
    for (int i(0); i < _tableModelsCount; ++i)
    {
      TableModelUtils::convertTableModel<Real, Rational>(_tableModels[i]);
    }
    updateTableViewsDelegates();
  }
}

void MainWindow::on_rationalRadioButton_toggled(bool checked)
{
  if (checked)
  {
    _currentField = Field::Rational;
    for (int i(0); i < _tableModelsCount; ++i)
    {
      TableModelUtils::convertTableModel<Rational, Real>(_tableModels[i]);
    }
    updateTableViewsDelegates();
  }
}

void MainWindow::on_action_Open_triggered()
{
  QString fileName(
    QFileDialog::getOpenFileName(
      this,
      QStringLiteral("Open Data File"),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/data.json",
      QStringLiteral("JSON files (*.json);;Text files (*.txt)"),
      0,
      QFileDialog::DontUseNativeDialog
    )
  );

  OperationResult result(loadData(fileName));
  switch (result)
  {
    case OperationResult::Success:
      setWindowFilePath(fileName);
      setWindowModified(true); //TODO: ~
      break;
    case OperationResult::Fail:
      QMessageBox::critical(
        this,
        "File Reading Error",
        "Could not open selected file."
      );
      break;
    default:
    case OperationResult::Nothing:
      break;
  }
}

void MainWindow::on_action_Save_as_triggered()
{
  QString fileName(
      QFileDialog::getSaveFileName(
        this,
        QStringLiteral("Save Data File As..."),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
        QString("/data@%1.json").arg(QDateTime::currentDateTime().toString(QStringLiteral("dMMMyy_h-m-s"))),
        QStringLiteral("JSON files (*.json);;Text files (*.txt)"),
        0,
        QFileDialog::DontUseNativeDialog
      )
  );

  OperationResult result(saveData(fileName));
  switch (result)
  {
    case OperationResult::Success:
      setWindowModified(false);
      break;
    case OperationResult::Fail:
      QMessageBox::critical(
        this,
        "File Writing Error",
        "Could not save file to the selected location."
      );
    case OperationResult::Nothing:
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
  clearModelsContents();
}

void MainWindow::on_action_Fill_w_random_numbers_triggered()
{
  TableModelUtils::fillTableModel(_tableModels[int(ModelType::ObjFunc)], TableModelUtils::FillMethod::Random);
  TableModelUtils::fillTableModel(_tableModels[int(ModelType::Constrs)], TableModelUtils::FillMethod::Random);
  TableModelUtils::fillTableModel(_tableModels[int(ModelType::RHS)], TableModelUtils::FillMethod::Random);
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
      "<h3><b>Linear Optimization v. 0.0.1</b></h3>"
      "<br><br>"
      "Copyright (c) 2015 Alexey Gorishny (<a href=\"http://www.0x414c.tk\">Homepage</a>)"
      "<br><br>"
      "Application for demonstrating some methods of solving linear optimization problems."
      "<br><br>"
      "This software uses:"
      "<ul>"
      "<li><a href=\"http://www.qt.io\">Qt (v. 5.5.1)</a></li>"
      "<li><a href=\"http://eigen.tuxfamily.org\">Eigen (v. 3.2.7)</a></li>"
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
  clearModelsContents();
}

void MainWindow::on_plotPushButton_clicked()
{
  switch (_currentField)
  {
    case Field::Real:
      {
        Matrix<Real, 1, Dynamic> objFuncCoeffs(TableModelUtils::getRowVector<Real>(_tableModels[int(ModelType::ObjFunc)]));
        Matrix<Real, Dynamic, Dynamic> constrsCoeffs(TableModelUtils::getMatrix<Real>(_tableModels[int(ModelType::Constrs)]));
        Matrix<Real, Dynamic, 1> constrsRHS(TableModelUtils::getColumnVector<Real>(_tableModels[int(ModelType::RHS)]));

        LinearProgramData<Real> linearProgramData(objFuncCoeffs, constrsCoeffs, constrsRHS);
        GraphicalSolver2D<Real> graphicalSolver2D(linearProgramData);
        optional<PlotData2D> plotData2D(graphicalSolver2D.solve());
        if (plotData2D)
        {
          plotGraph(*plotData2D);
        }
        else
        {
          goto error;
        }
        break;
      }
    case Field::Rational:
      {
        Matrix<Rational, 1, Dynamic> objFuncCoeffs(TableModelUtils::getRowVector<Rational>(_tableModels[int(ModelType::ObjFunc)]));
        Matrix<Rational, Dynamic, Dynamic> constrsCoeffs(TableModelUtils::getMatrix<Rational>(_tableModels[int(ModelType::Constrs)]));
        Matrix<Rational, Dynamic, 1> constrsRHS(TableModelUtils::getColumnVector<Rational>(_tableModels[int(ModelType::RHS)]));

        LinearProgramData<Rational> linearProgramData(objFuncCoeffs, constrsCoeffs, constrsRHS);
        GraphicalSolver2D<Rational> graphicalSolver2D(linearProgramData);
        optional<PlotData2D> plotData2D(graphicalSolver2D.solve());
        if (plotData2D)
        {
          plotGraph(*plotData2D);
        }
        else
        {
          goto error;
        }
        break;
      }
    error:
      QMessageBox::warning(
        this,
        QStringLiteral("Oops..."),
        QStringLiteral("This linear optimization problem can not be solved by 2D graphical method.")
      );
      break;
    default:
      break;
  }
}

void MainWindow::on_startPushButton_clicked()
{
  switch (_currentField)
  {
    case Field::Real:
      {
        Matrix<Real, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::getRowVector<Real>(_tableModels[int(ModelType::ObjFunc)])
        );
        Matrix<Real, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::getMatrix<Real>(_tableModels[int(ModelType::Constrs)])
        );
        Matrix<Real, Dynamic, 1> constrsRHS(
          TableModelUtils::getColumnVector<Real>(_tableModels[int(ModelType::RHS)])
        );

        LinearProgramData<Real> linearProgramData(objFuncCoeffs, constrsCoeffs, constrsRHS);
        _realSolver->setLinearProgramData(linearProgramData);
        optional<LinearProgramSolution<Real>> linearProgramSolution(_realSolver->solve());

        if (linearProgramSolution)
        {
          LOG("{0}=>{1}", (*linearProgramSolution).extremePoint,
              (*linearProgramSolution).extremeValue);
        }
        else
        {
          goto error;
        }
        break;
      }
    case Field::Rational:
      {
        Matrix<Rational, 1, Dynamic> objFuncCoeffs =
            TableModelUtils::getRowVector<Rational>(_tableModels[int(ModelType::ObjFunc)]);
        Matrix<Rational, Dynamic, Dynamic> constrsCoeffs =
            TableModelUtils::getMatrix<Rational>(_tableModels[int(ModelType::Constrs)]);
        Matrix<Rational, Dynamic, 1> constrsRHS =
            TableModelUtils::getColumnVector<Rational>(_tableModels[int(ModelType::RHS)]);

        LinearProgramData<Rational> linearProgramData(objFuncCoeffs, constrsCoeffs, constrsRHS);
        _rationalSolver->setLinearProgramData(linearProgramData);
        optional<LinearProgramSolution<Rational>> linearProgramSolution = _rationalSolver->solve();

        if (linearProgramSolution)
        {
          LOG("{0}=>{1}", (*linearProgramSolution).extremePoint,
              (*linearProgramSolution).extremeValue);
        }
        else
        {
          goto error;
        }
        break;
      }
    error:
      QMessageBox::warning(
        this,
        QStringLiteral("Oops..."),
        QStringLiteral("This linear optimization problem can not be solved by simplex method.")
      );
      break;
    default:
      break;
  }
}

[[deprecated("To be removed in release version!")]]
void MainWindow::on_testPushButton_clicked()
{
  {
//    _loadData(QStringLiteral("data4.json"));
//    loadData(QStringLiteral("data3+.json"));
    loadData(QStringLiteral("data5.json"));
  }

  {
//    real_t pi(3.141'592'653'589'793'238'46);
//    real_t sq_2(1.414'213'562'373'095'048'80);
//    real_t e(2.718'281'828'459'045'235'36);
//    for (int exp(0); exp < 17; ++exp)
//    {
//      real_t tol(pow(.1, exp));
//      auto rat_1 = MathUtils::rationalize<int32_t>(sq_2, tol);
//      cerr << exp << "=>" << tol << "==>" << rat_1.first << "/" << rat_1.second << ";\t\t" <<
//              endl << std::flush;
//    }
  }

  {
//    Matrix<Real, Dynamic, Dynamic> m(TableModelUtils::getMatrix<Real>(_models[int(Model::Constrs)]));
//    Matrix<Real, Dynamic, Dynamic> m(3, 4);
//    m << 0, 2, -1, -4, 2, 3, -1, -11, -2, 0, -3, 22;
//    m << 1, 2, -1, -4, 2, 3, -1, -11, -2, 0, -3, 22;
//    Matrix<Real, 2, 2> m(2, 2);
//    m << 3.7, -8.3, 2.3, -7.2;
//    auto rref(reducedRowEchelonForm<Real>(m));
//    LOG("M=\n{0}\nA=\n{1}\nP=\n{2}\n",
//      m.format(MathematicaFormat),
//      rref.first.format(MathematicaFormat),
//      rref.second.format(MathematicaFormat)
//    );
  }
}

void MainWindow::runSolver() {
  //TODO: !
}

void MainWindow::setupCustomPlot(QCustomPlot* const customPlot)
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
void MainWindow::plotGraph(const PlotData2D& plotData2D) {
  auto const customPlot = ui->customPlot;
  setupCustomPlot(customPlot);

  const QVector<QColor> plotColors{
    Qt::red, Qt::green, Qt::blue,
    Qt::cyan, Qt::magenta, Qt::yellow
  };
  const int colorsCount(plotColors.length());
//  const QFont normalFont(font().family(), 9);
  const QFont boldFont(font().family(), 9, QFont::Bold);
  const int verticesCount(plotData2D.vertices.length());

  if (verticesCount == 0)
  {
    return;
  }

  //Add heatmap for obj. func. values
//  QCPColorMap* colorMap = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
//  int nx(2);
//  int ny(2);
//  colorMap->data()->setSize(nx, ny);
//  colorMap->data()->setRange(QCPRange(0, 4), QCPRange(0, 3));
//  double x, y, z;
//  for (int xIdx(0); xIdx < nx; ++xIdx)
//  {
//    for (int yIdx(0); yIdx <ny; ++yIdx)
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
  QVector<Real> x1(verticesCount + 1), y1(verticesCount + 1);
  for (int i(0); i < verticesCount; ++i)
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
  for (int i(0); i < verticesCount; ++i) {
    //Constraint as infinite line
    QCPItemStraightLine* const constraintAsContiniousLine = new QCPItemStraightLine(customPlot);
    constraintAsContiniousLine->setPen(QPen(Qt::gray));
    constraintAsContiniousLine->point1->setCoords(plotData2D.vertices[i]);
    constraintAsContiniousLine->point2->setCoords(plotData2D.vertices[(i + 1) % verticesCount]);
    constraintAsContiniousLine->setSelectable(false);
    customPlot->addItem(constraintAsContiniousLine);

    //Constraint as feasible region edge
    QVector<Real> x(2), y(2); //add edge data
    x[0] = plotData2D.vertices[i].x();
    x[1] = plotData2D.vertices[(i + 1) % verticesCount].x();
    y[0] = plotData2D.vertices[i].y();
    y[1] = plotData2D.vertices[(i + 1) % verticesCount].y();
    customPlot->addGraph(); //create graph and assign data to it
    customPlot->graph(i)->setData(x, y);
    auto defaultColor(plotColors[i % colorsCount]); //set graph look
//    auto brushColor(defaultColor); brushColor.setAlphaF(.25);
//    auto selectedBrushColor(brushColor); selectedBrushColor.setAlphaF(.125);
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
  Qt::AlignmentFlag vertAlignment(plotData2D.gradient.p2().y() >= 0. ? Qt::AlignBottom : Qt::AlignTop);
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
