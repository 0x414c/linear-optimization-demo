#pragma once

#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX


#include <cstdint>

#include <memory>

#include <QCloseEvent>
#include <QMainWindow>
#include <QString>
#include <QWidget>

#include "qcustomplot/qcustomplot.h"

#include "numericstyleditemdelegate.hxx"
#include "simpletablemodel.hxx"
#include "../lp/dantzignumericsolver.hxx"
#include "../lp/dantzignumericsolvercontroller.hxx"
#include "../lp/inumericsolver.hxx"
#include "../lp/plotdata2d.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"


namespace Ui
{
  class MainWindow;
}


namespace GUI
{
  using namespace LinearProgramming;
  using namespace NumericTypes;
  using Utils::OperationResult;

  template<class T>
  using shared_ptr = std::shared_ptr<T>;


  class MainWindow : public QMainWindow
  {
    Q_OBJECT


    public:
      explicit MainWindow(QWidget* parent = 0);

      ~MainWindow();

      virtual void closeEvent(QCloseEvent* closeEvent) override;


    private slots:
      void on_customPlot_selectionChangedByUser();
      void on_customPlot_mousePress();
      void on_customPlot_mouseWheel();

      void on_anyProgramModel_dataChanged(
        const QModelIndex & topLeft, const QModelIndex & bottomRight,
        const QVector<int> & roles = QVector<int> ()
      );

      void on_constrsSpinBox_valueChanged(int arg1);
      void on_varsSpinBox_valueChanged(int arg1);

      void on_realRadioButton_toggled(bool checked);
      void on_rationalRadioButton_toggled(bool checked);

      void on_solveSimplexPushButton_clicked();
      void on_solveGraphicalPushButton_clicked();
      void on_clearPushButton_clicked();
      void on_testPushButton_clicked();

      void on_startSimplexPushButton_clicked();
      void on_stepBackSimplexPushButton_clicked();
      void on_stepForwardSimplexPushButton_clicked();

      void on_autoPivotSimplexCheckBox_toggled(bool checked);

      void on_action_Open_triggered();
      void on_action_Save_as_triggered();
      void on_action_Quit_triggered();

      void on_action_Undo_triggered();
      void on_action_Redo_triggered();

      void on_action_Clear_triggered();
      void on_action_Fill_w_random_numbers_triggered();

      void on_action_Zoom_in_triggered();
      void on_action_Zoom_out_triggered();
      void on_action_Zoom_reset_triggered();

      void on_action_How_to_triggered();
      void on_action_About_triggered();
      void on_action_About_Qt_triggered();


    private:
      enum struct ProgramModel : int
      { ObjFunc = 0, ConstrsCoeffs = 1, RHS = 2 };

      enum struct SimplexModel : int
      { Solution = 0, ObjFuncValue = 1, SimplexTableau = 2 };

      enum struct DetailsView : int
      { Graphical = 0, Simplex = 1 };


      Ui::MainWindow* ui;

      Field _currentField = Field::Real;

//      TODO: ~ `dataChanged' -> `_isDirty' =>
//      reload data only when it is needed
      bool _isDirty = false;

      QVector<SimpleTableModel*> _programTableModels;
      QVector<SimpleTableModel*> _simplexTableModels;

      QVector<NumericStyledItemDelegate<Real>*> _realNumericDelegates;
      QVector<NumericStyledItemDelegate<Rational>*> _ratNumericDelegates;

      shared_ptr<DantzigNumericSolver<Real>> _realSolver;
      shared_ptr<DantzigNumericSolver<Rational>> _rationalSolver;

      DantzigNumericSolverController<Real> _realSolverController;
      DantzigNumericSolverController<Rational> _rationalSolverController;

      void setupControlsDefaults();
      void setupSignals();

      void setDirty(bool dirty = true);

      void setupProgramView();
      void destroyProgramView();

      void enableGraphicalSolutionView(bool enabled = true);

      void setupSimplexView();
      void clearSimplexView();
      void refreshSimplexView();
      void updateSimplexSelectionRules();
      void destroySimplexView();
      void enableStepByStepSimplexView(bool enabled = true);
      void enableCurrentSolutionView(bool enabled = true);

      void clearModelsContents();
      void assignTableModelsHeaders();
      void convertTableModelsContents();
      void toggleTableViewsDelegates();
      void toggleField();

      void setupNumericSolvers();
      void updateNumericSolversData();
      void setupNumericSolversControllers();

      void setupCustomPlot(QCustomPlot* const customPlot);
      void plotGraph(const PlotData2D& plotData2D);

      OperationResult loadData(const QString& fileName);
      OperationResult saveData(const QString& fileName);
  };
}


#endif // MAINWINDOW_HXX
