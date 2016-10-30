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
#include <QWheelEvent>
#include <QWidget>

#include "qcustomplot/qcustomplot.h"

#include "numericstyleditemdelegate.hxx"
#include "stringtablemodel.hxx"
#include "../lp/simplexsolver.hxx"
#include "../lp/simplexsolvercontroller.hxx"
#include "../lp/plotdata2d.hxx"
#include "../lp/solutiontype.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"


namespace Gui
{
  using LinearProgramming::SimplexSolver;
  using LinearProgramming::SimplexSolverController;
  using LinearProgramming::PlotData2D;
  using LinearProgramming::SolutionType;
  using NumericTypes::Field;
  using NumericTypes::rational_t;
  using NumericTypes::real_t;
  using Utils::ResultType;
  using std::shared_ptr;


  namespace Ui
  {
    class MainWindow;
  }


  class MainWindow :
    public QMainWindow
  {
    Q_OBJECT


    public:
      explicit MainWindow(QWidget* parent = 0);

      ~MainWindow();

      virtual void closeEvent(QCloseEvent* ev) override;

      virtual void dragEnterEvent(QDragEnterEvent* ev) override;
      virtual void dragMoveEvent(QDragMoveEvent* ev) override;
      virtual void dragLeaveEvent(QDragLeaveEvent* ev) override;
      virtual void dropEvent(QDropEvent* ev) override;


    protected:
      virtual void changeEvent (QEvent* ev) override;


    private Q_SLOTS:
      void on_graphical_solutionPlotQCustomPlot_selectionChangedByUser();
      void on_graphical_solutionPlotQCustomPlot_mousePress(QMouseEvent* ev);
      void on_graphical_solutionPlotQCustomPlot_mouseWheel(QWheelEvent* ev);
      void on_graphical_solutionPlotQCustomPlot_mouseMove(QMouseEvent* ev);

      void on_anyProgramModel_dataChanged(
        const QModelIndex& topLeft, const QModelIndex& bottomRight,
        const QVector<int>& roles = QVector<int> ()
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
      {
        ObjFunc = 0,
        Constrs = 1,
        RHS = 2
      };


      enum struct SimplexModel : int
      {
        Solution = 0,
        ObjectiveValue = 1,
        Tableau = 2
      };


      enum struct DetailsView : int
      {
        Graphical = 0,
        Simplex = 1
      };


      Ui::MainWindow* ui = 0;

      Field field_ = Field::Real;

      //TODO: ~ `_isDirty' <- `dataChanged' =>
      //reload data only when it is needed.
      bool isDirty_ = false;

      bool isLoaded_ = false;

      QVector<StringTableModel*> programTableModels_;
      QVector<StringTableModel*> simplexTableModels_;

      QVector<NumericStyledItemDelegate<real_t>*> realNumericDelegates_;
      QVector<NumericStyledItemDelegate<rational_t>*> rationalNumericDelegates_;

      shared_ptr<SimplexSolver<real_t>> realSimplexSolver_;
      shared_ptr<SimplexSolver<rational_t>> rationalSimplexSolver_;

      SimplexSolverController<real_t> realSimplexSolverController_;
      SimplexSolverController<rational_t> rationalSimplexSolverController_;


      void setupDefaults();
      void setupSignals();

      void setupProgramView();
      void clearProgramView();
      void destroyProgramView();

      void setupGraphicalSolutionView(QCustomPlot* customPlot);
      void enableGraphicalSolutionView(bool enabled = true);
      void clearGraphicalSolutionView(QCustomPlot* customPlot);

      template<typename T = real_t>
      void refreshGraphicalSolutionView(const PlotData2D<T>& plotData2D);

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

      void solveSimplex();
      void solveGraphical();

      void solutionErrorHandler(const QString& description, SolutionType type);
      void pivotingErrorHandler(const QString& description, SolutionType type);

      void openFile(const QString& filename);
      void saveFile(const QString& filename);

      ResultType loadDataFromFile(const QString& fileName);
      ResultType saveDataToFile(const QString& fileName);

      void loadSettings();
      void saveSettings();
  };
}


#endif // MAINWINDOW_HXX
