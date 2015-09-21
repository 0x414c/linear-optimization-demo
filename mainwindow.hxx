#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>

namespace Ui {
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

    void on_clearPushButton_clicked();
    void on_plotPushButton_clicked();
    void on_startPushButton_clicked();
    void on_testPushButton_clicked();

    void on_customPlot_selectionChangedByUser();
    void on_customPlot_mousePress();
    void on_customPlot_mouseWheel();

    void on_action_How_to_triggered();
    void on_action_About_triggered();
    void on_action_About_Qt_triggered();
    void on_action_Zoom_in_triggered();
    void on_action_Zoom_out_triggered();
    void on_action_Undo_triggered();
    void on_action_Redo_triggered();
    void on_action_Clear_triggered();
    void on_action_Fill_w_random_numbers_triggered();
    void on_action_Open_triggered();
    void on_action_Save_triggered();
    void on_action_Quit_triggered();

  private:
    Ui::MainWindow* ui;

    void clearContents();
    void initializeContents();
    void solve();
    void plot();
};

#endif // MAINWINDOW_HXX
