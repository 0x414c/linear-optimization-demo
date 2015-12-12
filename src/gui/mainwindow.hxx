#pragma once

#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX


#include <cstdint>

#include <memory>

#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMainWindow>
#include <QMouseEvent>
#include <QString>
#include <QWidget>

#include "qcustomplot/qcustomplot.h"

#include "numericstyleditemdelegate.hxx"
#include "simpletablemodel.hxx"
#include "../lp/dantzignumericsolver.hxx"
#include "../lp/dantzignumericsolvercontroller.hxx"
#include "../lp/plotdatareal2d.hxx"
#include "../lp/solutiontype.hxx"
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
  using Utils::ResultType;

  template<class T>
  using shared_ptr = std::shared_ptr<T>;


  class MainWindow :
    public QMainWindow
  {
    Q_OBJECT


    public:
      explicit MainWindow(QWidget* parent = 0);

      ~MainWindow();

      virtual void closeEvent(QCloseEvent* closeEvent) override;

      virtual void dragEnterEvent(QDragEnterEvent* event) override;
      virtual void dragMoveEvent(QDragMoveEvent* event) override;
      virtual void dragLeaveEvent(QDragLeaveEvent* event) override;
      virtual void dropEvent(QDropEvent* event) override;


    private slots:
      void on_graphical_solutionPlotQCustomPlot_selectionChangedByUser();
      void on_graphical_solutionPlotQCustomPlot_mousePress();
      void on_graphical_solutionPlotQCustomPlot_mouseWheel();
      void on_graphical_solutionPlotQCustomPlot_mouseMove(
        QMouseEvent* mouseEvent
      );

      void on_anyProgramModel_dataChanged(
        const QModelIndex & topLeft, const QModelIndex & bottomRight,
        const QVector<int> & roles = QVector<int> ()
      );
      void on_anyProgramModel_dimensionsChanged(
        const QModelIndex& parent, int start, int end
      );
      void on_anyProgramModel_modelReset();

      void on_program_constrsSpinBox_valueChanged(int arg1);
      void on_program_varsSpinBox_valueChanged(int arg1);

      void on_program_realRadioButton_toggled(bool checked);
      void on_program_rationalRadioButton_toggled(bool checked);

      void on_control_solveSimplexPushButton_clicked();
      void on_control_solveGraphicalPushButton_clicked();
      void on_control_clearPushButton_clicked();
      void on_control_testPushButton_clicked();

      void on_simplex_startPushButton_clicked();
      void on_simplex_stepBackPushButton_clicked();
      void on_simplex_stepForwardPushButton_clicked();

      void on_simplex_manualPivotCheckBox_toggled(bool checked);
      void on_simplex_pivotHintPushButton_clicked();

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

      void on_action_Solve_triggered();
      void on_action_Plot_triggered();

      void on_action_How_to_triggered();
      void on_action_About_triggered();
      void on_action_About_Qt_triggered();


    private:
      enum struct ProgramModel : int
      { ObjFunc = 0, Constrs = 1, RHS = 2 };

      enum struct SimplexModel : int
      { Solution = 0, ObjectiveValue = 1, Tableau = 2 };

      enum struct DetailsView : int
      { Graphical = 0, Simplex = 1 };


      Ui::MainWindow* ui = 0;

      Field _field = Field::Real;

//      TODO: ~ `_isDirty' <- `dataChanged' =>
//      reload data only when it is needed
      bool _isDirty = false;

      bool _isLoaded = false;

      QVector<SimpleTableModel*> _programTableModels;
      QVector<SimpleTableModel*> _simplexTableModels;

      QVector<NumericStyledItemDelegate<Real>*> _realNumericDelegates;
      QVector<NumericStyledItemDelegate<Rational>*> _ratNumericDelegates;

      shared_ptr<DantzigNumericSolver<Real>> _realSolver;
      shared_ptr<DantzigNumericSolver<Rational>> _rationalSolver;

      DantzigNumericSolverController<Real> _realSolverController;
      DantzigNumericSolverController<Rational> _rationalSolverController;


      void setupDefaults();
      void setupSignals();

      void setupProgramView();
      void clearProgramView();
      void destroyProgramView();

      void setupGraphicalSolutionView(QCustomPlot* const customPlot);
      void enableGraphicalSolutionView(bool enabled = true);
      void clearGraphicalSolutionView(QCustomPlot* const customPlot);
      void refreshGraphicalSolutionView(const PlotDataReal2D& plotData2D);

      void setupSimplexView();
      void enableStepByStepSimplexView(bool enabled = true);
      void enableCurrentSolutionSimplexView(bool enabled = true);
      void updateSimplexSelectionRules();
      void clearSimplexView();
      void refreshSimplexView();
      void destroySimplexView();

      void assignTableModelsHeaders();
      void convertTableModelsContents();
      void toggleTableViewsDelegates();
      void toggleField();

      void setField(Field field);

      void setDirty(bool dirty = true);

      void setupNumericSolvers();
      void setupNumericSolversControllers();
      void updateNumericSolversData();

      void solveSimplexHandler();
      void solveGraphicalHandler();

      void solutionErrorHandler(const QString& description, SolutionType type);
      void pivotingErrorHandler(const QString& description, SolutionType type);

      void openFileHandler(const QString& filename);
      void saveFileHandler(const QString& filename);

      ResultType loadData(const QString& fileName);
      ResultType saveData(const QString& fileName);

      void loadSettings();
      void saveSettings();
  };
}


#endif // MAINWINDOW_HXX
