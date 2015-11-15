﻿#include "mainwindow.hxx"

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
#include "eigen3/Eigen/Core"
#include "qcustomplot/qcustomplot.h"

#include "../config.hxx"
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
using namespace Config::GUI;
using namespace Eigen;
using namespace std;
using namespace Utils;

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  setupProgramView();
  setupSimplexView();

  clearModelsContents();
  setTablesHeaders();
  updateTableViewsDelegates();
}

MainWindow::~MainWindow()
{
  destroyProgramView();
  destroySimplexView();

  delete ui;
}

void
MainWindow::setupProgramView()
{
  _ratNumericDelegates =
    QVector<NumericStyledItemDelegate<Rational>*>(ProgramModelsCount);
  _realNumericDelegates =
    QVector<NumericStyledItemDelegate<Real>*>(ProgramModelsCount);
  _programTableModels = QVector<SimpleTableModel*>(ProgramModelsCount);
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    _realNumericDelegates[i] = new NumericStyledItemDelegate<Real>(this);
    _ratNumericDelegates[i] = new NumericStyledItemDelegate<Rational>(this);
    _programTableModels[i] = new SimpleTableModel(this);
  }

  _programTableModels[int(ProgramModelType::ObjFunc)]->insertRow(0);
  _programTableModels[int(ProgramModelType::ObjFunc)]->insertColumns(0, 2);
  ui->objFuncCoeffsTableView->setModel(
    _programTableModels[int(ProgramModelType::ObjFunc)]
  );

  _programTableModels[int(ProgramModelType::ConstrsCoeffs)]->insertRows(0, 2);
  _programTableModels[int(ProgramModelType::ConstrsCoeffs)]->insertColumns(0, 2);
  ui->constrsCoeffsTableView->setModel(
    _programTableModels[int(ProgramModelType::ConstrsCoeffs)]
  );

  _programTableModels[int(ProgramModelType::RHS)]->insertRows(0, 2);
  _programTableModels[int(ProgramModelType::RHS)]->insertColumn(0);
  ui->constrsRHSTableView->setModel(
    _programTableModels[int(ProgramModelType::RHS)]
  );
}

void
MainWindow::destroyProgramView()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    delete _realNumericDelegates[i];
    delete _ratNumericDelegates[i];
    delete _programTableModels[i];
  }
}

void
MainWindow::clearModelsContents()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    _programTableModels[i]->clear(QStringLiteral("0"));
  }

  for (int i(0); i < SimplexModelsCount; ++i)
  {
    _simplexTableModels[i]->clear(QStringLiteral("0"));
  }
}

void
MainWindow::setTablesHeaders()
{
  _programTableModels[int(ProgramModelType::ObjFunc)]->setHeaderData(
    0, Qt::Vertical, QStringLiteral("c")
  );
  _programTableModels[int(ProgramModelType::RHS)]->setHeaderData(
    0, Qt::Horizontal, QStringLiteral("β")
  );

  _simplexTableModels[int(SimplexModelType::Solution)]->setHeaderData(
    0, Qt::Vertical, QStringLiteral("x*")
  );
  _simplexTableModels[int(SimplexModelType::FuncValue)]->setHeaderData(
    0, Qt::Vertical, QStringLiteral("w*")
  );
}

void
MainWindow::updateTableViewsDelegates()
{
  switch (_currentField)
  {
    case Field::Real:
      qDebug() << "MainWindow::updateTableViewsDelegates:"
                  " switching field to `Real'";
      ui->objFuncCoeffsTableView->setItemDelegate(
        _realNumericDelegates[int(ProgramModelType::ObjFunc)]
      );
      ui->constrsCoeffsTableView->setItemDelegate(
        _realNumericDelegates[int(ProgramModelType::ConstrsCoeffs)]
      );
      ui->constrsRHSTableView->setItemDelegate(
        _realNumericDelegates[int(ProgramModelType::RHS)]
      );
      break;
    case Field::Rational:
      qDebug() << "MainWindow::updateTableViewsDelegates:"
                  " switching field to `Rational'";
      ui->objFuncCoeffsTableView->setItemDelegate(
        _ratNumericDelegates[int(ProgramModelType::ObjFunc)]
      );
      ui->constrsCoeffsTableView->setItemDelegate(
        _ratNumericDelegates[int(ProgramModelType::ConstrsCoeffs)]
      );
      ui->constrsRHSTableView->setItemDelegate(
        _ratNumericDelegates[int(ProgramModelType::RHS)]
      );
      break;
    default:
      qWarning() << "MainWindow::updateTableViewsDelegates:"
                    " unknown value of `_currentField'";
      break;
  }
}

OperationResult
MainWindow::loadData(const QString& fileName)
{
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
      qWarning() << "MainWindow::loadData: couldn't open file"
                    " at path" << fileName;

      return OperationResult::Nothing;
    }
    else
    {
      const QByteArray byteArray(file.readAll());
      if (byteArray.count() > 0)
      {
        const QJsonDocument jsonDocument(QJsonDocument::fromJson(byteArray));
        TableModelStorage tableModelStorage;
        const OperationResult res(
          tableModelStorage.read(jsonDocument.object())
        );
        file.flush();
        file.close();

        if (res == OperationResult::Success)
        {
          if (tableModelStorage.count() == ProgramModelsCount)
          {
            for (int i(0); i < ProgramModelsCount; ++i)
            {
              //Transfer contents...
              (*_programTableModels[i]) = std::move(tableModelStorage[i]);
               //...and update ownerhsip
              (*_programTableModels[i]).setParent(this);
            }
            setTablesHeaders();

            ui->varsSpinBox->setValue(
              _programTableModels[int(ProgramModelType::ConstrsCoeffs)]->columnCount()
            );
            ui->constrsSpinBox->setValue(
              _programTableModels[int(ProgramModelType::ConstrsCoeffs)]->rowCount()
            );

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
                qWarning() << "MainWindow::loadData: unknown value of `Field'";
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
          qWarning() << "MainWindow::loadData: couldn't correctly parse"
                        " file contents: the file is either incompatible"
                        " or invalid";

          return res;
        }
      }
      else
      {
        qWarning() << "MainWindow::loadData: couldn't read file contents:"
                      " the file is empty";

        return OperationResult::Nothing;
      }
    }
  }
  else
  {
    return OperationResult::Nothing;
  }
}

OperationResult
MainWindow::saveData(const QString& fileName)
{
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
      qWarning() << "MainWindow::saveData: couldn't save file"
                    " at path" << fileName;

      return OperationResult::Nothing;
    }
    else
    {
      QJsonObject jsonObject;
      //TODO: ?~ Use pointers here
      TableModelStorage tableModelStorage(
        QVector<SimpleTableModel>{
          (*_programTableModels[int(ProgramModelType::ObjFunc)]),
          (*_programTableModels[int(ProgramModelType::ConstrsCoeffs)]),
          (*_programTableModels[int(ProgramModelType::RHS)])
        },
        _currentField
      );
      const OperationResult res(tableModelStorage.write(jsonObject));

      if (res == OperationResult::Success)
      {
        const QJsonDocument jsonDocument(jsonObject);

        const qint64 bytesCount(
          file.write(jsonDocument.toJson(QJsonDocument::Indented))
        );
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

void
MainWindow::on_customPlot_selectionChangedByUser()
{
  if (
    ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
    ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
    ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
    ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)
  )
  {
    ui->customPlot->xAxis2->setSelectedParts(
      QCPAxis::spAxis | QCPAxis::spTickLabels
    );
    ui->customPlot->xAxis->setSelectedParts(
      QCPAxis::spAxis | QCPAxis::spTickLabels
    );
  }

  if (
    ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
    ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
    ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
    ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)
  )
  {
    ui->customPlot->yAxis2->setSelectedParts(
      QCPAxis::spAxis | QCPAxis::spTickLabels
    );
    ui->customPlot->yAxis->setSelectedParts(
      QCPAxis::spAxis | QCPAxis::spTickLabels
    );
  }

  for (int i(0); i < ui->customPlot->plottableCount(); ++i)
  {
    QCPAbstractPlottable* const plottable = ui->customPlot->plottable(i);
    QCPPlottableLegendItem* const item =
      ui->customPlot->legend->itemWithPlottable(plottable);
    if (item->selected() || plottable->selected())
    {
      item->setSelected(true);
      plottable->setSelected(true);
    }
  }
}

void
MainWindow::on_customPlot_mousePress()
{
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    ui->customPlot->axisRect()->setRangeDrag(
      ui->customPlot->xAxis->orientation()
    );
  }
  else
  {
    if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
      ui->customPlot->axisRect()->setRangeDrag(
        ui->customPlot->yAxis->orientation()
      );
    }
    else
    {
      ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    }
  }
}

void
MainWindow::on_customPlot_mouseWheel()
{
  if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
  {
    ui->customPlot->axisRect()->setRangeZoom(
      ui->customPlot->xAxis->orientation()
    );
  }
  else
  {
    if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    {
      ui->customPlot->axisRect()->setRangeZoom(
        ui->customPlot->yAxis->orientation()
      );
    }
    else
    {
      ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }
  }
}

void
MainWindow::on_constrsSpinBox_valueChanged(int arg1)
{
  if (arg1 <= MaxConstraints)
  {
    int oldRowsCount(_programTableModels[int(ProgramModelType::ConstrsCoeffs)]->rowCount());
    int colsCount(_programTableModels[int(ProgramModelType::ConstrsCoeffs)]->columnCount());

    _programTableModels[int(ProgramModelType::ConstrsCoeffs)]->resize(arg1, 0);
    _programTableModels[int(ProgramModelType::ConstrsCoeffs)]->clear(
      oldRowsCount, 0, arg1, colsCount, QStringLiteral("0")
    );

    _programTableModels[int(ProgramModelType::RHS)]->resize(arg1, 0);
    _programTableModels[int(ProgramModelType::RHS)]->clear(
      oldRowsCount, 0, arg1, 1, QStringLiteral("0")
    );
  }
}

void
MainWindow::on_varsSpinBox_valueChanged(int arg1)
{
  if (arg1 <= MaxVariables)
  {
    ui->constrsSpinBox->setMaximum(arg1);

    int oldColsCount(
      _programTableModels[int(ProgramModelType::ConstrsCoeffs)]->columnCount()
    );
    int rowsCount(
      _programTableModels[int(ProgramModelType::ConstrsCoeffs)]->rowCount()
    );

    _programTableModels[int(ProgramModelType::ConstrsCoeffs)]->resize(0, arg1);
    _programTableModels[int(ProgramModelType::ConstrsCoeffs)]->clear(
      0, oldColsCount, rowsCount, arg1, QStringLiteral("0")
    );

    _programTableModels[int(ProgramModelType::ObjFunc)]->resize(0, arg1);
    _programTableModels[int(ProgramModelType::ObjFunc)]->clear(
      0, oldColsCount, 1, arg1, QStringLiteral("0")
    );
  }
}

void
MainWindow::on_realRadioButton_toggled(bool checked)
{
  if (checked)
  {
    _currentField = Field::Real;
    for (int i(0); i < ProgramModelsCount; ++i)
    {
      TableModelUtils::convert<Real, Rational>(_programTableModels[i]);
    }
    updateTableViewsDelegates();
  }
}

void
MainWindow::on_rationalRadioButton_toggled(bool checked)
{
  if (checked)
  {
    _currentField = Field::Rational;
    for (int i(0); i < ProgramModelsCount; ++i)
    {
      TableModelUtils::convert<Rational, Real>(_programTableModels[i]);
    }
    updateTableViewsDelegates();
  }
}

void
MainWindow::on_action_Open_triggered()
{
  QString fileName(
    QFileDialog::getOpenFileName(
      this,
      QStringLiteral("Open Data File"),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
      "/data.json",
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
      //TODO: ~ Use `setWindowModified'
//      setWindowModified(true);
      break;
    case OperationResult::Fail:
      QMessageBox::critical(
        this,
        QStringLiteral("File Reading Error"),
        QStringLiteral("Could not open selected file.")
      );
      break;
    default:
    case OperationResult::Nothing:
      break;
  }
}

void
MainWindow::on_action_Save_as_triggered()
{
  QString fileName(
    QFileDialog::getSaveFileName(
      this,
      QStringLiteral("Save Data File As..."),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
      QString("/data@%1.json").arg(
        QDateTime::currentDateTime().toString(QStringLiteral("dMMMyy_h-m-s"))
      ),
      QStringLiteral("JSON files (*.json);;Text files (*.txt)"),
      0,
      QFileDialog::DontUseNativeDialog
    )
  );

  OperationResult result(saveData(fileName));
  switch (result)
  {
    case OperationResult::Success:
//      setWindowModified(false);
      break;
    case OperationResult::Fail:
      QMessageBox::critical(
        this,
        QStringLiteral("File Writing Error"),
        QStringLiteral("Could not save file to the selected location.")
      );
    case OperationResult::Nothing:
    default:
      break;
  }
}

void
MainWindow::on_action_Quit_triggered()
{
  QApplication::quit();
}

void
MainWindow::on_action_Undo_triggered()
{
  //TODO: ~ Undo action
}

void
MainWindow::on_action_Redo_triggered()
{
  //TODO: ~ Redo action
}

void
MainWindow::on_action_Clear_triggered()
{
  clearModelsContents();
}

void
MainWindow::on_action_Fill_w_random_numbers_triggered()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    TableModelUtils::fill(_programTableModels[i], TableModelUtils::FillMethod::Random);
  }
}

void
MainWindow::on_action_Zoom_in_triggered()
{
  //TODO: ~ Connect to QCP
}

void
MainWindow::on_action_Zoom_out_triggered()
{
  //TODO: ~ Connect to QCP
}

void
MainWindow::on_action_Zoom_reset_triggered()
{
  //TODO: ~ Connect to QCP
}

void
MainWindow::on_action_How_to_triggered()
{
  //TODO: ~ Add how-to guide
}

void
MainWindow::on_action_About_triggered()
{
  QMessageBox::about(
    this,
    QStringLiteral("About Application..."),
    QStringLiteral(
      "<h3><b>Linear Optimization v. 0.0.1</b></h3>"
      "<br><br>"
      "This software demonstrates some methods of solving of linear programs"
      "<br><br>"
      "This software uses:"
      "<ul>"
      "<li><a href=\"http://www.qt.io\">Qt (v. 5.5.1)</a></li>"
      "<li><a href=\"http://eigen.tuxfamily.org\">Eigen (v. 3.2.7)</a></li>"
      "<li><a href=\"http://www.qcustomplot.com\">"
      "QCustomPlot (v. 1.3.1)</a></li>"
      "<li><a href=\"http://www.boost.org\">Boost (v. 1.59.0)</a></li>"
      "<li><a href=\"http://cppformat.github.io\">"
      "C++ Format (v. 1.1.0)</a></li>"
      "<li><a href=\"http://git.gnome.org/browse/adwaita-icon-theme\">"
      "Adwaita Icon Theme (v. 3.16.2.1)</a>"
      " by the <a href=\"http://www.gnome.org\">GNOME Project</a></li>"
      //http://www.gnome.org
      //http://wiki.gnome.org/Design
      //http://git.gnome.org/browse/adwaita-icon-theme
      //http://ftp.gnome.org/pub/gnome/sources/adwaita-icon-theme/3.16
      //http://github.com/GNOME/adwaita-icon-theme'
      "</ul>"
      "<br><br>"
      "Copyright (C) 2015  Alexey Gorishny"
      "<br>"
      "This program is free software: you can redistribute it and/or modify"
      " it under the terms of the GNU General Public License as published by"
      " the Free Software Foundation, either version 3 of the License, or"
      " (at your option) any later version."
      "<br>"
      "This program is distributed in the hope that it will be useful,"
      " but WITHOUT ANY WARRANTY; without even the implied warranty of"
      " MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
      " GNU General Public License for more details."
      "<br>"
      "You should have received a copy of the GNU General Public License"
      " along with this program.  If not, see"
      " <a href=\"http://www.gnu.org/licenses\">Licenses"
      " - GNU Project - Free Software Foundation</a>."
    )
  );
}

void
MainWindow::on_action_About_Qt_triggered()
{
  QApplication::aboutQt();
}

void
MainWindow::on_clearPushButton_clicked()
{
  clearModelsContents();
}

void
MainWindow::on_plotPushButton_clicked()
{
  switch (_currentField)
  {
    case Field::Real:
      {
        Matrix<Real, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<Real>(
            _programTableModels[int(ProgramModelType::ObjFunc)]
          )
        );
        Matrix<Real, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<Real>(
            _programTableModels[int(ProgramModelType::ConstrsCoeffs)]
          )
        );
        Matrix<Real, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<Real>(
            _programTableModels[int(ProgramModelType::RHS)]
          )
        );

        LinearProgramData<Real> linearProgramData(
          std::move(objFuncCoeffs),
          std::move(constrsCoeffs),
          std::move(constrsRHS)
        );
        GraphicalSolver2D<Real> graphicalSolver2D(
          std::move(linearProgramData)
        );
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
        Matrix<Rational, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<Rational>(
            _programTableModels[int(ProgramModelType::ObjFunc)]
          )
        );
        Matrix<Rational, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<Rational>(
            _programTableModels[int(ProgramModelType::ConstrsCoeffs)]
          )
        );
        Matrix<Rational, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<Rational>(
            _programTableModels[int(ProgramModelType::RHS)]
          )
        );

        LinearProgramData<Rational> linearProgramData(
          std::move(objFuncCoeffs),
          std::move(constrsCoeffs),
          std::move(constrsRHS)
        );
        GraphicalSolver2D<Rational> graphicalSolver2D(
          std::move(linearProgramData)
        );
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
        QStringLiteral(
          "This linear program can not be solved by"
          " the 2D graphical method."
        )
      );
      break;
    default:
      break;
  }
}

void
MainWindow::on_startPushButton_clicked()
{
  switch (_currentField)
  {
    case Field::Real:
      {
        Matrix<Real, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<Real>(
            _programTableModels[int(ProgramModelType::ObjFunc)]
          )
        );
        Matrix<Real, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<Real>(
            _programTableModels[int(ProgramModelType::ConstrsCoeffs)]
          )
        );
        Matrix<Real, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<Real>(
            _programTableModels[int(ProgramModelType::RHS)]
          )
        );

        LinearProgramData<Real> linearProgramData(
          std::move(objFuncCoeffs),
          std::move(constrsCoeffs),
          std::move(constrsRHS)
        );
        _realSolver->setLinearProgramData(std::move(linearProgramData));
        optional<LinearProgramSolution<Real>> linearProgramSolution(
          _realSolver->solve()
        );

        if (linearProgramSolution)
        {
          LOG(
            "Solution: x* := {0}\nw* := {1}",
            (*linearProgramSolution).extremePoint,
            (*linearProgramSolution).extremeValue
          );
        }
        else
        {
          goto error;
        }
        break;
      }
    case Field::Rational:
      {
        Matrix<Rational, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<Rational>(
            _programTableModels[int(ProgramModelType::ObjFunc)]
          )
        );
        Matrix<Rational, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<Rational>(
            _programTableModels[int(ProgramModelType::ConstrsCoeffs)]
          )
        );
        Matrix<Rational, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<Rational>(
            _programTableModels[int(ProgramModelType::RHS)]
          )
        );

        LinearProgramData<Rational> linearProgramData(
          std::move(objFuncCoeffs),
          std::move(constrsCoeffs),
          std::move(constrsRHS)
        );
        _rationalSolver->setLinearProgramData(linearProgramData);
        optional<LinearProgramSolution<Rational>> linearProgramSolution(
          _rationalSolver->solve()
        );

        if (linearProgramSolution)
        {
          LOG(
            "Solution: x* := {0}\nw* := {1}",
            (*linearProgramSolution).extremePoint,
            (*linearProgramSolution).extremeValue
          );
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
        QStringLiteral("This linear program can not be solved by"
                       " the Simplex method.")
      );
      break;
    default:
      break;
  }
}

[[deprecated("To be removed in release version!")]]
void
MainWindow::on_testPushButton_clicked()
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
//      cerr << exp << "=>" << tol << "==>" << rat_1.first <<
//           "/" << rat_1.second << ";\t\t" <<
//              std::endl << std::flush;
//    }
  }

  {
//    Matrix<Real, Dynamic, Dynamic> m(
//      TableModelUtils::getMatrix<Real>(_models[int(Model::Constrs)])
//    );
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

void MainWindow::setupSimplexView()
{
//  enum struct SimplexModelType : int
//  { Solution = 0, FuncValue = 1, SimplexTableau = 2 };
  _simplexTableModels = QVector<SimpleTableModel*>(SimplexModelsCount);
  for (int i(0); i < SimplexModelsCount; ++i)
  {
    _simplexTableModels[i] = new SimpleTableModel(this);
  }
  _simplexTableModels[int(SimplexModelType::Solution)]->insertRow(0);
  _simplexTableModels[int(SimplexModelType::Solution)]->insertColumn(0);
  ui->solutionVectorTableView->setModel(
    _simplexTableModels[int(SimplexModelType::Solution)]
  );

  _simplexTableModels[int(SimplexModelType::FuncValue)]->insertRow(0);
  _simplexTableModels[int(SimplexModelType::FuncValue)]->insertColumn(0);
  ui->functionValueTableView->setModel(
    _simplexTableModels[int(SimplexModelType::FuncValue)]
  );

  _simplexTableModels[int(SimplexModelType::SimplexTableau)]->insertRow(0);
  _simplexTableModels[int(SimplexModelType::SimplexTableau)]->insertColumn(0);
  ui->simplexTableauTableView->setModel(
    _simplexTableModels[int(SimplexModelType::SimplexTableau)]
  );
}

void
MainWindow::refreshSimplexView()
{

}

void MainWindow::destroySimplexView()
{
  for (int i(0); i < SimplexModelsCount; ++i)
  {
    delete _simplexTableModels[i];
  }
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

  customPlot->xAxis->setSelectableParts(
    QCPAxis::spAxis | QCPAxis::spTickLabels
  );
  customPlot->yAxis->setSelectableParts(
    QCPAxis::spAxis | QCPAxis::spTickLabels
  );
  customPlot->xAxis->setRange(0, 2);
  customPlot->yAxis->setRange(0, 1);
  customPlot->xAxis->setLabel(QString("x%1").arg(1)); //TODO: ! Use proper names here
  customPlot->yAxis->setLabel(QString("x%1").arg(2)); //TODO: ! Use proper names here

  connect(
    customPlot, SIGNAL(selectionChangedByUser()),
    this, SLOT(on_customPlot_selectionChangedByUser())
  );
  connect(
    customPlot, SIGNAL(mousePress(QMouseEvent*)),
    this, SLOT(on_customPlot_mousePress())
  );
  connect(
    customPlot, SIGNAL(mouseWheel(QWheelEvent*)),
    this, SLOT(on_customPlot_mouseWheel())
  );
  connect(
    customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
    customPlot->xAxis2, SLOT(setRange(QCPRange))
  );
  connect(
    customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)),
    customPlot->yAxis2, SLOT(setRange(QCPRange))
  );
}

[[deprecated("Needs refactoring. How obvious, huh.")]]
void
MainWindow::plotGraph(const PlotData2D& plotData2D) {
  QCustomPlot* const customPlot = ui->customPlot;
  setupCustomPlot(customPlot);

  const QVector<QColor> plotColors{
    Qt::red, Qt::green, Qt::blue,
    Qt::cyan, Qt::magenta, Qt::yellow
  };
  const int colorsCount(plotColors.count());
//  const QFont normalFont(font().family(), 9);
  const QFont boldFont(font().family(), 9, QFont::Bold);
  const int verticesCount(plotData2D.vertices.count());

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
  QCPItemStraightLine* const xAxisPlotLine =
    new QCPItemStraightLine(customPlot);
  xAxisPlotLine->setPen(QPen(Qt::gray));
  xAxisPlotLine->point1->setCoords(QPointF(0, 0));
  xAxisPlotLine->point2->setCoords(QPointF(1, 0));
  xAxisPlotLine->setSelectable(false);
  customPlot->addItem(xAxisPlotLine);

  QCPItemStraightLine* const yAxisPlotLine =
    new QCPItemStraightLine(customPlot);
  yAxisPlotLine->setPen(QPen(Qt::gray));
  yAxisPlotLine->point1->setCoords(QPointF(0, 0));
  yAxisPlotLine->point2->setCoords(QPointF(0, 1));
  yAxisPlotLine->setSelectable(false);
  customPlot->addItem(yAxisPlotLine);

  //Add feasible region
  QCPCurve* const feasibleRegionCurve =
    new QCPCurve(customPlot->xAxis, customPlot->yAxis);
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
  feasibleRegionCurve->setScatterStyle(
    QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(127, 0, 127), 8)
  );
  feasibleRegionCurve->setName(QStringLiteral("U"));
  feasibleRegionCurve->setSelectable(true);
  customPlot->addPlottable(feasibleRegionCurve);

  //Add constraints
  for (int i(0); i < verticesCount; ++i) {
    //Constraint as infinite line
    QCPItemStraightLine* const constraintAsContiniousLine =
      new QCPItemStraightLine(customPlot);
    constraintAsContiniousLine->setPen(QPen(Qt::gray));
    constraintAsContiniousLine->point1->setCoords(plotData2D.vertices[i]);
    constraintAsContiniousLine->point2->setCoords(
      plotData2D.vertices[(i + 1) % verticesCount]
    );
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
  QCPItemStraightLine* const objFuncPlotLine =
    new QCPItemStraightLine(customPlot);
  objFuncPlotLine->setPen(QPen(Qt::darkBlue));
  objFuncPlotLine->point1->setCoords(levelLine.p1());
  objFuncPlotLine->point2->setCoords(levelLine.p2());
  objFuncPlotLine->setSelectable(true);
  customPlot->addItem(objFuncPlotLine);

  //HACK: Invisible level line (only a section) to obtain anchor for text label.
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
  QCPItemText* const objFuncGradientVectorArrowText =
    new QCPItemText(customPlot);
  Qt::AlignmentFlag vertAlignment(
    plotData2D.gradient.p2().y() >= 0. ? Qt::AlignBottom : Qt::AlignTop
  );
  objFuncGradientVectorArrowText->setPositionAlignment(
    vertAlignment | Qt::AlignHCenter
  );
  objFuncGradientVectorArrowText->position->setParentAnchor(
    objFuncGradientVectorArrow->end
  );
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
