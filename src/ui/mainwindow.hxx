#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <memory>

#include <QMainWindow>
#include <QStandardItemModel>
#include <QString>
#include <QWidget>

#include "qcustomplot/qcustomplot.h"

#include "numericstyleditemdelegate.hxx"
#include "tablemodel.hxx"
#include "../lp/dantzignumericsolver.hxx"
#include "../lp/inumericsolver.hxx"
#include "../lp/plotdata2d.hxx"
#include "../misc/utils.hxx"

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
    void on_coeffsSpinBox_valueChanged(int arg1);

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

    const int _modelsCount = 3;
    enum struct Model : int { ObjFunc = 0, Constrs = 1, RHS = 2 };

    Field _field = Field::Real;
    QVector<TableModel*> _models;
    QVector<NumericStyledItemDelegate<real_t>*> _realNumericDelegates;
    QVector<NumericStyledItemDelegate<rational_t>*> _ratNumericDelegates;

    unique_ptr<DantzigNumericSolver<real_t>> _realSolver = make_unique<DantzigNumericSolver<real_t>>();
    unique_ptr<DantzigNumericSolver<rational_t>> _ratSolver = make_unique<DantzigNumericSolver<rational_t>>();

    DialogResult _loadData(const QString& fileName);
    DialogResult _saveData(const QString& fileName);

    void _initializeWindowContents();
    void _deleteWindowContents();

    void _clearModelsContents();
    void _setTablesHeaders();
    void _updateTableViewsDelegates();
    void _setupCustomPlot(QCustomPlot* const customPlot);

    void _runSolver();
    void _plotGraph(const PlotData2D& plotData2D);
};

#endif // MAINWINDOW_HXX
