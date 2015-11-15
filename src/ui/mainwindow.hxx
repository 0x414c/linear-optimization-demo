#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <memory>

#include <QMainWindow>
#include <QString>
#include <QWidget>

#include "qcustomplot/qcustomplot.h"

#include "numericstyleditemdelegate.hxx"
#include "tablemodel.hxx"
#include "../lp/dantzignumericsolver.hxx"
#include "../lp/inumericsolver.hxx"
#include "../lp/plotdata2d.hxx"
#include "../math/numerictypes.hxx"
#include "../misc/utils.hxx"

using namespace LinearProgramming;
using namespace NumericTypes;
using namespace std;
using namespace Utils;

namespace Ui
{
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

  private slots:
    void on_constrsSpinBox_valueChanged(int arg1);
    void on_varsSpinBox_valueChanged(int arg1);

    void on_realRadioButton_toggled(bool checked);
    void on_rationalRadioButton_toggled(bool checked);

    void on_clearPushButton_clicked();
    void on_plotPushButton_clicked();
    void on_startPushButton_clicked();
    void on_testPushButton_clicked();

    void on_customPlot_selectionChangedByUser();
    void on_customPlot_mousePress();
    void on_customPlot_mouseWheel();

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
    Ui::MainWindow* ui;

    Field _currentField = Field::Real;

    enum struct ProgramModelType : int
    { ObjFunc = 0, ConstrsCoeffs = 1, RHS = 2 };

    enum struct SimplexModelType : int
    { Solution = 0, FuncValue = 1, SimplexTableau = 2 };

    QVector<SimpleTableModel*> _programTableModels;
    QVector<SimpleTableModel*> _simplexTableModels;

    QVector<NumericStyledItemDelegate<Real>*> _realNumericDelegates;
    QVector<NumericStyledItemDelegate<Rational>*> _ratNumericDelegates;

    unique_ptr<DantzigNumericSolver<Real>> _realSolver =
        make_unique<DantzigNumericSolver<Real>>();
    unique_ptr<DantzigNumericSolver<Rational>> _rationalSolver =
        make_unique<DantzigNumericSolver<Rational>>();

    OperationResult loadData(const QString& fileName);
    OperationResult saveData(const QString& fileName);

    void setupProgramView();
    void destroyProgramView();

    void clearModelsContents();
    void setTablesHeaders();
    void updateTableViewsDelegates();

    void setupSimplexView();
    void refreshSimplexView();
    void destroySimplexView();

    void setupCustomPlot(QCustomPlot* const customPlot);
    void plotGraph(const PlotData2D& plotData2D);
};

#endif // MAINWINDOW_HXX
