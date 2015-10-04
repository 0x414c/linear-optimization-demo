#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <memory>
#include <cmath>

#include <QMainWindow>
#include <QStandardItemModel>
#include <QString>
#include <QWidget>

#include "inumericsolver.hxx"
#include "dantzignumericsolver.hxx"
#include "utils.hxx"

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

    ComputationMode _mode = ComputationMode::Real;

    unique_ptr<DantzigNumericSolver<real_t>> _realSolver = make_unique<DantzigNumericSolver<real_t>>();
    unique_ptr<DantzigNumericSolver<rat_t>> _ratSolver = make_unique<DantzigNumericSolver<rat_t>>();

    void _clearTablesContents();
    void _setTablesValidators();
    void _setTablesHeaders();
    void _initializeWindowContents();

    DialogResult _loadData(QString fileName);
    DialogResult _saveData(QString fileName);

    void _runSolver();
    void _plotGraph();
};

#endif // MAINWINDOW_HXX
