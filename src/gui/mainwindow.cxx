#include "mainwindow.hxx"

#include "ui_mainwindow.h"

#include <cmath>
#include <cstddef>

#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include <QBrush>
#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileInfo>
#include <QItemSelectionModel>
#include <QLocale>
#include <QList>
#include <QMessageBox>
#include <QMimeData>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPen>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QVector>
#include <QWheelEvent>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"
#include "fmt/format.h"
#include "qcustomplot/qcustomplot.h"

#include "numericstyleditemdelegate.hxx"
#include "stringtablemodel.hxx"
#include "tablemodelcollection.hxx"
#include "tablemodelutils.hxx"
#include "../lp/simplexsolver.hxx"
#include "../lp/simplexsolvercontroller.hxx"
#include "../lp/graphicalsolver2d.hxx"
#include "../lp/linearprogrammingutils.hxx"
#include "../lp/linearprogramsolution.hxx"
#include "../lp/plotdata2d.hxx"
#include "../lp/solutiontype.hxx"
#include "../math/mathutils.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"
#include "../config.hxx"
#include "../globaldefinitions.hxx"


namespace Gui
{
  using boost::optional;
  using namespace Config::App;
  using namespace Config::Gui;
  using DataConvertors::numericCast;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using fmt::format;
  using LinearProgramming::GraphicalSolver2D;
  using LinearProgramming::LinearProgramData;
  using LinearProgramming::LinearProgramSolution;
  using LinearProgramming::MaybeIndex2D;
  using LinearProgramming::SimplexTableau;
  using LinearProgrammingUtils::blerp;
  using LinearProgrammingUtils::perp;
  using Utils::ResultType;
  using std::make_shared;
  using std::pair;
  using std::vector;
}


#pragma region ctors/dtors

Gui::MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  setupNumericSolvers();
  setupNumericSolversControllers();

  setupProgramView();
  setupSimplexView();

  setupDefaults();
  setupSignals();

  clearProgramView();
  clearSimplexView();

  toggleTableViewsDelegates();
  assignTableModelsHeaders();

  loadSettings();

  isLoaded_ = true;
}


Gui::MainWindow::~MainWindow()
{
  saveSettings();

  destroySimplexView();
  destroyProgramView();

  delete ui;
}

#pragma endregion // ctors/dtors


#pragma region event handlers

void
Gui::MainWindow::closeEvent(QCloseEvent* ev)
{
  //TODO: ~ Check if the content was _really_ modified.
  if (isWindowModified()) {
  begin:
    const QMessageBox::StandardButton res(
      QMessageBox::question(
        this,
        QStringLiteral("Unsaved Changes"),
        QString(
          "You have some unsaved changes in the program \"%1\"."
          "\n\nDo you want to save them?"
        ).
        arg(QFileInfo(windowFilePath()).baseName()),
        QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard,
        QMessageBox::Cancel
      )
    );

    switch (res) {
      case QMessageBox::Save:
        Q_EMIT on_action_Save_as_triggered();

        if (!isWindowModified())
        {
          ev->accept();

          break;
        }
        else
        {
          goto begin;
        }

      case QMessageBox::Discard:
        ev->accept();
        break;

      case QMessageBox::Cancel:
        ev->ignore();
        break;

      default:
        break;
    }
  }
  else
  {
    ev->accept();
  }
}


void
Gui::MainWindow::dragEnterEvent(QDragEnterEvent* ev)
{
  if (ev->mimeData()->hasUrls())
  {
    ev->acceptProposedAction();
  }
  else
  {
    ev->ignore();
  }
}


void
Gui::MainWindow::dragMoveEvent(QDragMoveEvent* ev)
{
  if (ev->mimeData()->hasUrls())
  {
    ev->acceptProposedAction();
  }
  else
  {
    ev->ignore();
  }
}


void
Gui::MainWindow::dragLeaveEvent(QDragLeaveEvent* ev)
{
  ev->accept();
}


void
Gui::MainWindow::dropEvent(QDropEvent* ev)
{
  const QMimeData* const mimeData = ev->mimeData();

  if (mimeData->hasUrls())
  {
    const QList<QUrl> urls(mimeData->urls());
    for (int i(0); i < urls.count() && i < 1; ++i)
    {
      const QUrl url(urls.at(i));
      const QFileInfo fileInfo(url.toLocalFile());
      if (fileInfo.isFile())
      {
        const QString suffix(fileInfo.suffix());
        if (
          (suffix.compare(QStringLiteral("json"), Qt::CaseInsensitive) == 0) ||
          (suffix.compare(QStringLiteral("txt"), Qt::CaseInsensitive) == 0)
        )
        {
          openFile(fileInfo.filePath());
          ev->acceptProposedAction();
        }
        else
        {
          ev->ignore();
        }
      }
      else
      {
        ev->ignore();
      }
    }
  }
  else
  {
    ev->ignore();
  }
}


void
Gui::MainWindow::changeEvent(QEvent* ev)
{
  QMainWindow::changeEvent(ev);
  switch (ev->type())
  {
    case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;

    default:
      break;
  }
}

#pragma endregion // event handlers


#pragma region private methods

void
Gui::MainWindow::setupDefaults()
{
  ui->program_constrsSpinBox->setMinimum(MinConstraints);
  ui->program_constrsSpinBox->setMaximum(MaxConstraints);
  ui->program_constrsSpinBox->setValue(DefaultConstraints);

  ui->program_varsSpinBox->setMinimum(MinVariables);
  ui->program_varsSpinBox->setMaximum(MaxVariables);
  ui->program_varsSpinBox->setValue(DefaultVariables);

  ui->detailsTabWidget->setCurrentIndex(int(DetailsView::Graphical));

  ui->graphicalMethodTab->setEnabled(false);
  ui->simplexMethodTab->setEnabled(false);

  enableGraphicalSolutionView(false);

  enableStepByStepSimplexView(false);
  enableCurrentSolutionSimplexView(false);

  ui->program_realRadioButton->setChecked(true);

  ui->simplex_manualPivotCheckBox->setChecked(false);

  setWindowFilePath(DefaultDocumentTitle);
}


void
Gui::MainWindow::setupSignals()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    connect(
      programTableModels_[i],
      SIGNAL(
        dataChanged(
          const QModelIndex&, const QModelIndex&, const QVector<int>&
        )
      ),
      this,
      SLOT(
        on_anyProgramModel_dataChanged(
          const QModelIndex&, const QModelIndex&, const QVector<int>&
        )
      )
    );

    connect(
      programTableModels_[i],
      SIGNAL(rowsInserted(const QModelIndex&, int, int)),
      this,
      SLOT(on_anyProgramModel_dimensionsChanged(const QModelIndex&, int, int))
    );

    connect(
      programTableModels_[i],
      SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
      this,
      SLOT(on_anyProgramModel_dimensionsChanged(const QModelIndex&, int, int))
    );

    connect(
      programTableModels_[i],
      SIGNAL(columnsInserted(const QModelIndex&, int, int)),
      this,
      SLOT(on_anyProgramModel_dimensionsChanged(const QModelIndex&, int, int))
    );

    connect(
      programTableModels_[i],
      SIGNAL(columnsRemoved(const QModelIndex&, int, int)),
      this,
      SLOT(on_anyProgramModel_dimensionsChanged(const QModelIndex&, int, int))
    );

    connect(
      programTableModels_[i], SIGNAL(modelReset()),
      this, SLOT(on_anyProgramModel_modelReset())
    );
  }
}


void
Gui::MainWindow::setupProgramView()
{
  rationalNumericDelegates_ =
    QVector<NumericStyledItemDelegate<rational_t>*>(ProgramModelsCount);

  realNumericDelegates_ =
    QVector<NumericStyledItemDelegate<real_t>*>(ProgramModelsCount);

  programTableModels_ = QVector<StringTableModel*>(ProgramModelsCount);
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    realNumericDelegates_[i] = new NumericStyledItemDelegate<real_t>(this);
    rationalNumericDelegates_[i] = new NumericStyledItemDelegate<rational_t>(this);
    programTableModels_[i] = new StringTableModel(this);
  }

  programTableModels_[int(ProgramModel::ObjFunc)]->insertRow(0);
  programTableModels_[int(ProgramModel::ObjFunc)]->insertColumns(
    0, DefaultVariables
  );
  ui->program_objFuncCoeffsTableView->setModel(
    programTableModels_[int(ProgramModel::ObjFunc)]
  );

  programTableModels_[int(ProgramModel::Constrs)]->insertRows(
    0, DefaultConstraints
  );
  programTableModels_[int(ProgramModel::Constrs)]->insertColumns(
    0, DefaultVariables
  );
  ui->program_constrsCoeffsTableView->setModel(
    programTableModels_[int(ProgramModel::Constrs)]
  );

  programTableModels_[int(ProgramModel::RHS)]->insertRows(
    0, DefaultConstraints
  );
  programTableModels_[int(ProgramModel::RHS)]->insertColumn(0);
  ui->program_constrsRHSTableView->setModel(
    programTableModels_[int(ProgramModel::RHS)]
  );
}


void
Gui::MainWindow::clearProgramView()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    programTableModels_[i]->clear(QStringLiteral("0"));
  }
}


void
Gui::MainWindow::destroyProgramView()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    delete realNumericDelegates_[i];
    delete rationalNumericDelegates_[i];
    delete programTableModels_[i];
  }
}


void
Gui::MainWindow::setupGraphicalSolutionView(QCustomPlot* customPlot)
{
  clearGraphicalSolutionView(customPlot);

  //Set up all the things
  const QFont normalFont(font().family(), FontSize);
  const QFont boldFont(font().family(), FontSize, QFont::Bold);

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

  customPlot->xAxis->setRange(0., 1.);
  customPlot->yAxis->setRange(0., 1.);

  customPlot->xAxis->setLabel(QStringLiteral("x1"));
  customPlot->yAxis->setLabel(QStringLiteral("x2"));

  customPlot->setBackgroundScaled(true);
  customPlot->setBackgroundScaledMode(Qt::IgnoreAspectRatio);

  connect(
    customPlot, SIGNAL(selectionChangedByUser()),
    this, SLOT(on_graphical_solutionPlotQCustomPlot_selectionChangedByUser())
  );

  connect(
    customPlot, SIGNAL(mousePress(QMouseEvent*)),
    this, SLOT(on_graphical_solutionPlotQCustomPlot_mousePress(QMouseEvent*))
  );

  connect(
    customPlot, SIGNAL(mouseWheel(QWheelEvent*)),
    this,
    SLOT(on_graphical_solutionPlotQCustomPlot_mouseWheel(QWheelEvent*))
  );

  connect(
    customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
    customPlot->xAxis2, SLOT(setRange(QCPRange))
  );

  connect(
    customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)),
    customPlot->yAxis2, SLOT(setRange(QCPRange))
  );

  connect(
    customPlot, SIGNAL(mouseMove(QMouseEvent*)),
    this, SLOT(on_graphical_solutionPlotQCustomPlot_mouseMove(QMouseEvent*))
  );
}


void
Gui::MainWindow::enableGraphicalSolutionView(bool enabled)
{
  if (enabled)
  {
    ui->graphical_solutionPlotQCustomPlot->setBackground(Qt::white);
  }
  else
  {
    ui->graphical_solutionPlotQCustomPlot->setBackground(Qt::lightGray);
  }

  ui->graphical_solutionPlotQCustomPlot->setEnabled(enabled);
}


void
Gui::MainWindow::clearGraphicalSolutionView(QCustomPlot* customPlot)
{
  customPlot->clearFocus();
  customPlot->clearGraphs();
  customPlot->clearItems();
  customPlot->clearMask();
  customPlot->clearPlottables();

  //Remove `ColorScale' at the second column
  customPlot->plotLayout()->remove(customPlot->plotLayout()->element(0, 1));
  //Force layout reflow
  customPlot->plotLayout()->simplify();

  customPlot->replot();
}


template<typename T>
void
Gui::MainWindow::refreshGraphicalSolutionView(const PlotData2D<T>& plotData) {
  QCustomPlot* const customPlot = ui->graphical_solutionPlotQCustomPlot;

  setupGraphicalSolutionView(customPlot);

  const QFont normalFont(font().family(), FontSize);
  const QFont boldFont(font().family(), FontSize, QFont::Bold);

  const QVector<QColor> colors{
    Qt::red, Qt::green, Qt::blue,
    Qt::cyan, Qt::magenta, Qt::yellow
  };

  const int colorsCount(colors.count());

  //TODO: ~ Avoid copying.
  const vector<Matrix<real_t, 2, 1>> vertices(
    plotData.feasibleRegionExtremePoints.cbegin(),
    plotData.feasibleRegionExtremePoints.cend()
  );

  const size_t verticesCount(plotData.feasibleRegionExtremePoints.size());

  //Add colormap for obj. func. values `F^'...
  const int xSize(
    ColorMapResolution * (
      plotData.feasibleRegionBoundingBox(0, 1) -
      plotData.feasibleRegionBoundingBox(0, 0) + .5
    )
  );
  const int ySize(
    ColorMapResolution * (
      plotData.feasibleRegionBoundingBox(1, 1) -
      plotData.feasibleRegionBoundingBox(1, 0) + .5
    )
  );

  QCPColorMap* const colorMap =
    new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
  colorMap->data()->setSize(xSize, ySize);
  colorMap->data()->setRange(
    QCPRange(
      qreal(plotData.feasibleRegionBoundingBox(0, 0)),
      qreal(plotData.feasibleRegionBoundingBox(0, 1))
    ),
    QCPRange(
      qreal(plotData.feasibleRegionBoundingBox(1, 0)),
      qreal(plotData.feasibleRegionBoundingBox(1, 1))
    )
  );

  {
    using std::placeholders::_1;
    using std::placeholders::_2;

    const auto blerpOverBoundingBox(
      bind(
        blerp,
        plotData.feasibleRegionBoundingBox(0, 0),
        plotData.feasibleRegionBoundingBox(1, 0),
        plotData.feasibleRegionBoundingBoxHeights(1, 0),
        plotData.feasibleRegionBoundingBoxHeights(0, 0),
        plotData.feasibleRegionBoundingBox(0, 1),
        plotData.feasibleRegionBoundingBox(1, 1),
        plotData.feasibleRegionBoundingBoxHeights(0, 1),
        plotData.feasibleRegionBoundingBoxHeights(1, 1),
        _1, _2
      )
    );

    qreal x0, y0;
    for (int xIdx(0); xIdx < xSize; ++xIdx)
    {
      for (int yIdx(0); yIdx < ySize; ++yIdx)
      {
        colorMap->data()->cellToCoord(xIdx, yIdx, &x0, &y0);
        const qreal z0(blerpOverBoundingBox(x0, y0));
        colorMap->data()->setCell(xIdx, yIdx, z0);
      }
    }
  }

  //w/ a corresponding color scale
  QCPColorScale* const colorScale = new QCPColorScale(customPlot);
  colorScale->setType(QCPAxis::atRight);
  colorMap->setColorScale(colorScale);
  colorScale->axis()->setLabel("F^");
  customPlot->plotLayout()->insertColumn(1);
  customPlot->plotLayout()->addElement(0, 1, colorScale);

  QCPColorGradient colorGradient;
  colorGradient.setColorStops(ColorMap_PuBlGrYe);
  colorGradient.setLevelCount(ColorMapLevelsCount);
  colorGradient.setColorInterpolation(QCPColorGradient::ciHSV);
  colorMap->setGradient(colorGradient);

  colorMap->setInterpolate(true);
  colorMap->setTightBoundary(true);
  colorMap->rescaleDataRange(true);
  colorMap->setName(QStringLiteral("F^"));
  colorMap->updateLegendIcon();
  colorMap->setSelectable(true);
  customPlot->addPlottable(colorMap);

  //Make sure that the axis rect and color scale synchronize their
  //bottom and top margins (so they line up)
  QCPMarginGroup* const marginGroup = new QCPMarginGroup(customPlot);
  customPlot->axisRect()->setMarginGroup(
    QCP::msBottom | QCP::msTop, marginGroup
  );
  colorScale->setMarginGroup(QCP::msBottom | QCP::msTop, marginGroup);

  //Add horizontal axis...
  QCPItemStraightLine* const xAxisPlotLine =
    new QCPItemStraightLine(customPlot);
  xAxisPlotLine->setPen(QPen(Qt::gray));
  xAxisPlotLine->point1->setCoords(QPointF(0., 0.));
  xAxisPlotLine->point2->setCoords(QPointF(1., 0.));
  xAxisPlotLine->setSelectable(false);
  customPlot->addItem(xAxisPlotLine);

  //...and the vertical one
  QCPItemStraightLine* const yAxisPlotLine =
    new QCPItemStraightLine(customPlot);
  yAxisPlotLine->setPen(QPen(Qt::gray));
  yAxisPlotLine->point1->setCoords(QPointF(0., 0.));
  yAxisPlotLine->point2->setCoords(QPointF(0., 1.));
  yAxisPlotLine->setSelectable(false);
  customPlot->addItem(yAxisPlotLine);

  //Set axes names according to solution
  customPlot->xAxis->setLabel(
    QString("x%1").arg(plotData.decisionVariables.at(0))
  );
  customPlot->yAxis->setLabel(
    QString("x%1").arg(plotData.decisionVariables.at(1))
  );

  //Add feasible region `Q'
  QCPCurve* const feasibleRegionCurve =
    new QCPCurve(customPlot->xAxis, customPlot->yAxis);
  QVector<qreal> x1(verticesCount + 1), y1(verticesCount + 1);
  for (size_t i(0); i < verticesCount; ++i)
  {
    x1[i] = qreal(vertices[i].x());
    y1[i] = qreal(vertices[i].y());
  }
  x1[verticesCount] = qreal(vertices.front().x());
  y1[verticesCount] = qreal(vertices.front().y());
  feasibleRegionCurve->setData(x1, y1);
  feasibleRegionCurve->setPen(QPen(Qt::transparent));
  feasibleRegionCurve->setBrush(QBrush(QColor(31, 31, 31, 31)));
  feasibleRegionCurve->setSelectedPen(QPen(Qt::transparent));
  feasibleRegionCurve->setSelectedBrush(QBrush(QColor(31, 31, 31, 95)));
  feasibleRegionCurve->setScatterStyle(
    QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(255, 0, 0), 8)
  );
  feasibleRegionCurve->setName(QStringLiteral("Q"));
  feasibleRegionCurve->setSelectable(true);
  customPlot->addPlottable(feasibleRegionCurve);

  //Add constraints
  for (size_t i(0); i < verticesCount; ++i) {
    //Add constraint as infinite line
    QCPItemStraightLine* const constraintStraightLine =
      new QCPItemStraightLine(customPlot);
    constraintStraightLine->setPen(QPen(Qt::gray));
    constraintStraightLine->point1->setCoords(
      QPointF(qreal(vertices[i].x()), qreal(vertices[i].y()))
    );
    constraintStraightLine->point2->setCoords(
      QPointF(
        qreal(vertices[(i + 1) % verticesCount].x()),
        qreal(vertices[(i + 1) % verticesCount].y())
      )
    );
    constraintStraightLine->setSelectable(false);
    customPlot->addItem(constraintStraightLine);

    //Constraint as a feasible region edge
    QVector<qreal> x(2), y(2);
    x[0] = qreal(vertices[i].x());
    x[1] = qreal(vertices[(i + 1) % verticesCount].x());
    y[0] = qreal(vertices[i].y());
    y[1] = qreal(vertices[(i + 1) % verticesCount].y());
    customPlot->addGraph();
    customPlot->graph(i)->setData(x, y);
    auto defaultColor(colors[i % colorsCount]);
//    auto brushColor(defaultColor); brushColor.setAlphaF(.25);
//    auto selectedBrushColor(brushColor); selectedBrushColor.setAlphaF(.125);
    customPlot->graph(i)->setPen(QPen(defaultColor));
//    customPlot->graph(i)->setBrush(QBrush(brushColor));
    customPlot->graph(i)->setSelectedPen(
      QPen(QBrush(defaultColor), SelectedPenWidth)
    );
//    customPlot->graph(i)->setSelectedBrush(QBrush(selectedBrushColor));
    customPlot->graph(i)->setName(QString("Eq. (%1)").arg(i + 1));
    //TODO: ~ Use string representation (e.g. 2x + 3y = 0)
    customPlot->graph(i)->setSelectable(true);
  }

  //Add objective function level line...
  const Matrix<real_t, 2, 1> levelLine(perp(plotData.gradientVector));
  QCPItemStraightLine* const objFuncPlotLine =
    new QCPItemStraightLine(customPlot);
  objFuncPlotLine->point1->setCoords(0., 0.);
  objFuncPlotLine->point2->setCoords(
    qreal(levelLine.x()), qreal(levelLine.y())
  );
  objFuncPlotLine->setPen(QPen(Qt::darkBlue));
  objFuncPlotLine->setSelectedPen(
    QPen(QBrush(Qt::darkBlue), SelectedPenWidth)
  );
  objFuncPlotLine->setSelectable(true);
  customPlot->addItem(objFuncPlotLine);

  //HACK: Invisible level line (only a section) to obtain anchor for text label
  QCPItemLine* const levelLineSection = new QCPItemLine(customPlot);
  levelLineSection->start->setCoords(0., 0.);
  levelLineSection->end->setCoords(qreal(levelLine.x()), qreal(levelLine.y()));
  levelLineSection->setPen(QPen(Qt::transparent));
  levelLineSection->setSelectable(false);
  customPlot->addItem(levelLineSection);

  //...w/ text label
  QCPItemText* const objFuncLevelLineText = new QCPItemText(customPlot);
  objFuncLevelLineText->setPositionAlignment(Qt::AlignTop | Qt::AlignRight);
  objFuncLevelLineText->position->setParentAnchor(levelLineSection->start);
//  objFuncLevelLineText->position->setType(QCPItemPosition::ptPlotCoords);
  objFuncLevelLineText->position->setCoords(-1., 0.);
//  objFuncLevelLineText->setRotation(levelLine.angle());
  objFuncLevelLineText->setText(QStringLiteral(" F(⃗x) = const "));
  objFuncLevelLineText->setFont(boldFont);
  objFuncLevelLineText->setSelectable(false);
  customPlot->addItem(objFuncLevelLineText);

  //Add gradient vector...
  QCPItemLine* const objFuncGradientVectorArrow = new QCPItemLine(customPlot);
  objFuncGradientVectorArrow->start->setCoords(QPointF(0., 0.));
  objFuncGradientVectorArrow->end->setCoords(
    qreal(plotData.gradientVector.x()), qreal(plotData.gradientVector.y())
  );
  objFuncGradientVectorArrow->setHead(QCPLineEnding::esLineArrow);
  objFuncGradientVectorArrow->setPen(QPen(Qt::darkRed));
  objFuncGradientVectorArrow->setSelectedPen(
    QPen(QBrush(Qt::darkRed), SelectedPenWidth)
  );
  objFuncGradientVectorArrow->setSelectable(true);
  customPlot->addItem(objFuncGradientVectorArrow);

  //...w/ text label
  QCPItemText* const objFuncGradientVectorArrowText =
    new QCPItemText(customPlot);
  const Qt::AlignmentFlag vertAlignment(
    plotData.gradientVector.y() >= 0. ? Qt::AlignBottom : Qt::AlignTop
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
    QString::fromStdString(
      format(
        " ∇F(⃗x) = ({}; {}) ",
        plotData.gradientVector.x(),
        plotData.gradientVector.y()
      )
    )
  );
  objFuncGradientVectorArrowText->setSelectable(false);
  customPlot->addItem(objFuncGradientVectorArrowText);

  //Add text label w/ objective value at the top
  QCPItemText* const objectiveValueText = new QCPItemText(customPlot);
  objectiveValueText->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
  objectiveValueText->position->setType(QCPItemPosition::ptAxisRectRatio);
  objectiveValueText->position->setCoords(.33, .033);
  objectiveValueText->setFont(boldFont);
  objectiveValueText->setText(
    QString::fromStdString(
      format(
        " x* = {}\nF* = min F^ = {} ",
        plotData.linearProgramSolution.extremePoint.
        transpose().format(Utils::RowVectorFormat),
        plotData.linearProgramSolution.extremeValue
      )
    )
  );
  objectiveValueText->setPen(QPen(Qt::black)); //Show black border around text
  objectiveValueText->setSelectable(false);
  customPlot->addItem(objectiveValueText);

  //Add arrows pointing to the objective values
  for (
    auto it(plotData.extremePoints.cbegin());
    it != plotData.extremePoints.cend();
    ++it
  )
  {
    QCPItemLine* const objectiveValueArrow = new QCPItemLine(customPlot);
    objectiveValueArrow->start->setParentAnchor(objectiveValueText->bottomLeft);
    objectiveValueArrow->end->setCoords(qreal((*it).x()), qreal((*it).y()));
    objectiveValueArrow->setHead(QCPLineEnding::esLineArrow);
    objectiveValueArrow->setSelectable(false);
    customPlot->addItem(objectiveValueArrow);
  }

  //TODO: !? We can't use `Tracer' on a `Curve'
//  QCPItemTracer* const phaseTracer = new QCPItemTracer(customPlot);
//  phaseTracer->setGraph(customPlot->graph(1));
//  phaseTracer->setGraphKey(0.);
//  phaseTracer->setInterpolating(true);
//  phaseTracer->setStyle(QCPItemTracer::tsCircle);
//  phaseTracer->setPen(QPen(Qt::red));
//  phaseTracer->setBrush(Qt::red);
//  phaseTracer->setSize(8);
//  customPlot->addItem(phaseTracer);

  //Finalize
  customPlot->rescaleAxes();
  customPlot->replot();
}


void
Gui::MainWindow::setupSimplexView()
{
  simplexTableModels_ = QVector<StringTableModel*>(SimplexModelsCount);
  for (int i(0); i < SimplexModelsCount; ++i)
  {
    simplexTableModels_[i] = new StringTableModel(this);
  }

  simplexTableModels_[int(SimplexModel::Solution)]->insertRow(0);
  simplexTableModels_[int(SimplexModel::Solution)]->insertColumn(0);
  simplexTableModels_[int(SimplexModel::Solution)]->setEditable(false);
  simplexTableModels_[int(SimplexModel::Solution)]->setSelectable(false);
  ui->simplex_solutionVectorTableView->setModel(
    simplexTableModels_[int(SimplexModel::Solution)]
  );

  simplexTableModels_[int(SimplexModel::ObjectiveValue)]->insertRow(0);
  simplexTableModels_[int(SimplexModel::ObjectiveValue)]->insertColumn(0);
  simplexTableModels_[int(SimplexModel::ObjectiveValue)]->setEditable(false);
  simplexTableModels_[int(SimplexModel::ObjectiveValue)]->setSelectable(false);
  ui->simplex_objectiveValueTableView->setModel(
    simplexTableModels_[int(SimplexModel::ObjectiveValue)]
  );

  simplexTableModels_[int(SimplexModel::Tableau)]->insertRow(0);
  simplexTableModels_[int(SimplexModel::Tableau)]->insertColumn(0);
  simplexTableModels_[int(SimplexModel::Tableau)]->setEditable(false);
  ui->simplex_simplexTableauTableView->setModel(
    simplexTableModels_[int(SimplexModel::Tableau)]
  );
}


void
Gui::MainWindow::enableStepByStepSimplexView(bool enabled)
{
  ui->simplex_startPushButton->setEnabled(enabled);
  ui->simplex_stepBackPushButton->setEnabled(enabled);
  ui->simplex_stepForwardPushButton->setEnabled(enabled);
  ui->simplex_iterCountLcdNumber->setEnabled(enabled);
  ui->simplex_manualPivotCheckBox->setEnabled(enabled);
  ui->simplex_pivotHintPushButton->setEnabled(enabled);
  ui->simplex_simplexTableauTableView->setEnabled(enabled);
}


void
Gui::MainWindow::enableCurrentSolutionSimplexView(bool enabled)
{
  ui->simplex_solutionVectorTableView->setEnabled(enabled);
  ui->simplex_objectiveValueTableView->setEnabled(enabled);
}


void
Gui::MainWindow::updateSimplexSelectionRules()
{
  //TODO: ~
}


void
Gui::MainWindow::clearSimplexView()
{
  for (int i(0); i < SimplexModelsCount; ++i)
  {
    simplexTableModels_[i]->clear(QStringLiteral("0"));
  }
}


void
Gui::MainWindow::refreshSimplexView()
{
  switch (field_)
  {
    case Field::Real:
      if (
        !realSimplexSolverController_.isEmpty() &&
        realSimplexSolverController_.stateChanged()
      )
      {
        const SimplexTableau<real_t> tableau(realSimplexSolverController_.current());

        simplexTableModels_[int(SimplexModel::Tableau)]->resize(
          tableau.rows(), tableau.cols()
        );

        TableModelUtils::fill<real_t>(
          simplexTableModels_[int(SimplexModel::Tableau)],
          tableau
        );

        ui->simplex_simplexTableauTableView->resizeColumnsToContents();

        const Matrix<real_t, Dynamic, 1> extremePoint(tableau.extremePoint());

        simplexTableModels_[int(SimplexModel::Solution)]->resize(
          1, extremePoint.rows()
        );

        TableModelUtils::fill<real_t>(
          simplexTableModels_[int(SimplexModel::Solution)],
          extremePoint.transpose()
        );

        ui->simplex_solutionVectorTableView->resizeColumnsToContents();

        TableModelUtils::fill(
          simplexTableModels_[int(SimplexModel::ObjectiveValue)],
          numericCast<QString, real_t>(
            tableau.extremeValue()
          )
        );

        DenseIndex idx(0);
        const DenseIndex rows(tableau.rows());
        const DenseIndex cols(tableau.cols());
        TableModelUtils::setFlags<real_t>(
          simplexTableModels_[int(SimplexModel::Tableau)],
          tableau.entries(),
          [rows, cols, &idx, &tableau]
          (const real_t& x) {
            const DenseIndex rowIdx(idx / cols);
            const DenseIndex colIdx(idx % cols);
            const bool ret(
              ((rowIdx < rows - 1) && (colIdx < cols - 1)) &&
                MathUtils::isLessThanZero<real_t>(
                  tableau.row(tableau.rows() - 1)(colIdx)
                ) &&
                MathUtils::isGreaterThanZero<real_t>(x)
            );
            ++idx;

            return ret;
          },
          Qt::ItemIsEnabled | Qt::ItemIsSelectable
        );
      }

      ui->simplex_simplexTableauTableView->selectionModel()->clearSelection();
      ui->simplex_simplexTableauTableView->selectionModel()->clearCurrentIndex();

      ui->simplex_stepBackPushButton->setEnabled(
        realSimplexSolverController_.hasPrevious()
      );

      ui->simplex_stepForwardPushButton->setEnabled(
        realSimplexSolverController_.hasNext()
      );

      ui->simplex_iterCountLcdNumber->display(
        int(realSimplexSolverController_.iterationsCount())
      );

      ui->simplex_manualPivotCheckBox->setEnabled(
        realSimplexSolverController_.hasNext()
      );

      ui->simplex_pivotHintPushButton->setEnabled(
        realSimplexSolverController_.hasNext()
      );
      break;

    case Field::Rational:
      if (
        !rationalSimplexSolverController_.isEmpty() &&
        rationalSimplexSolverController_.stateChanged()
      )
      {
        const SimplexTableau<rational_t> tableau(
          rationalSimplexSolverController_.current()
        );

        simplexTableModels_[int(SimplexModel::Tableau)]->resize(
          tableau.rows(), tableau.cols()
        );

        TableModelUtils::fill<rational_t>(
          simplexTableModels_[int(SimplexModel::Tableau)],
          tableau
        );

        ui->simplex_simplexTableauTableView->resizeColumnsToContents();

        const Matrix<rational_t, Dynamic, 1> extremePoint(tableau.extremePoint());

        simplexTableModels_[int(SimplexModel::Solution)]->resize(
          1, extremePoint.rows()
        );

        TableModelUtils::fill<rational_t>(
          simplexTableModels_[int(SimplexModel::Solution)],
          extremePoint.transpose()
        );

        ui->simplex_solutionVectorTableView->resizeColumnsToContents();

        TableModelUtils::fill(
          simplexTableModels_[int(SimplexModel::ObjectiveValue)],
          numericCast<QString, rational_t>(
            tableau.extremeValue()
          )
        );

        DenseIndex idx(0);
        const DenseIndex rows(tableau.rows());
        const DenseIndex cols(tableau.cols());
        TableModelUtils::setFlags<rational_t>(
          simplexTableModels_[int(SimplexModel::Tableau)],
          tableau.entries(),
          [rows, cols, &idx, &tableau]
          (const rational_t& x) {
            const DenseIndex rowIdx(idx / cols);
            const DenseIndex colIdx(idx % cols);
            const bool ret(
              ((rowIdx < rows - 1) && (colIdx < cols - 1)) &&
                MathUtils::isLessThanZero<rational_t>(
                  tableau.row(tableau.rows() - 1)(colIdx)
                ) &&
                MathUtils::isGreaterThanZero<rational_t>(x)
            );
            ++idx;

            return ret;
          },
          Qt::ItemIsEnabled | Qt::ItemIsSelectable
        );
      }

      ui->simplex_simplexTableauTableView->selectionModel()->clearSelection();
      ui->simplex_simplexTableauTableView->selectionModel()->clearCurrentIndex();

      ui->simplex_stepBackPushButton->setEnabled(
        rationalSimplexSolverController_.hasPrevious()
      );

      ui->simplex_stepForwardPushButton->setEnabled(
        rationalSimplexSolverController_.hasNext()
      );

      ui->simplex_iterCountLcdNumber->display(
        int(rationalSimplexSolverController_.iterationsCount())
      );

      ui->simplex_manualPivotCheckBox->setEnabled(
        rationalSimplexSolverController_.hasNext()
      );

      ui->simplex_pivotHintPushButton->setEnabled(
        rationalSimplexSolverController_.hasNext()
      );
      break;

    default:
      break;
  }
}


void
Gui::MainWindow::destroySimplexView()
{
  for (int i(0); i < SimplexModelsCount; ++i)
  {
    delete simplexTableModels_[i];
  }
}


void
Gui::MainWindow::assignTableModelsHeaders()
{
  programTableModels_[int(ProgramModel::ObjFunc)]->setHeaderData(
    0, Qt::Vertical, QStringLiteral("c")
  );
  programTableModels_[int(ProgramModel::RHS)]->setHeaderData(
    0, Qt::Horizontal, QStringLiteral("β")
  );

  simplexTableModels_[int(SimplexModel::Solution)]->setHeaderData(
    0, Qt::Vertical, QStringLiteral("x*")
  );
  simplexTableModels_[int(SimplexModel::ObjectiveValue)]->setHeaderData(
    0, Qt::Vertical, QStringLiteral("F*")
  );
}


void
Gui::MainWindow::convertTableModelsContents()
{
  switch (field_)
  {
    case Field::Real:
      for (int i(0); i < ProgramModelsCount; ++i)
      {
        TableModelUtils::convert<real_t, rational_t>(programTableModels_[i]);
      }
      break;

    case Field::Rational:
      for (int i(0); i < ProgramModelsCount; ++i)
      {
        TableModelUtils::convert<rational_t, real_t>(programTableModels_[i]);
      }
      break;

    default:
      break;
  }
}


void
Gui::MainWindow::toggleTableViewsDelegates()
{
  switch (field_)
  {
    case Field::Real:
      qInfo() << "MainWindow::toggleTableViewsDelegates:"
                 " switching to `Field::Real'";
      ui->program_objFuncCoeffsTableView->setItemDelegate(
        realNumericDelegates_[int(ProgramModel::ObjFunc)]
      );
      ui->program_constrsCoeffsTableView->setItemDelegate(
        realNumericDelegates_[int(ProgramModel::Constrs)]
      );
      ui->program_constrsRHSTableView->setItemDelegate(
        realNumericDelegates_[int(ProgramModel::RHS)]
      );
      break;

    case Field::Rational:
      qInfo() << "MainWindow::toggleTableViewsDelegates:"
                  " switching to `Field::Rational'";
      ui->program_objFuncCoeffsTableView->setItemDelegate(
        rationalNumericDelegates_[int(ProgramModel::ObjFunc)]
      );
      ui->program_constrsCoeffsTableView->setItemDelegate(
        rationalNumericDelegates_[int(ProgramModel::Constrs)]
      );
      ui->program_constrsRHSTableView->setItemDelegate(
        rationalNumericDelegates_[int(ProgramModel::RHS)]
      );
      break;

    default:
      qCritical() << "MainWindow::toggleTableViewsDelegates:"
                     " unsupported value of `field_'";
      break;
  }
}


void
Gui::MainWindow::toggleField()
{
  convertTableModelsContents();
  toggleTableViewsDelegates();

  clearSimplexView();
  enableCurrentSolutionSimplexView(false);
  enableStepByStepSimplexView(false);
}


void
Gui::MainWindow::setField(Field field)
{
  if (field_ != field)
  {
    field_ = field;

    toggleField();
  }
}


void
Gui::MainWindow::setDirty(bool dirty)
{
  if (isDirty_ != dirty)
  {
    isDirty_ = dirty;

    setWindowModified(dirty);
  }
}


void
Gui::MainWindow::setupNumericSolvers()
{
  realSimplexSolver_ = make_shared<SimplexSolver<real_t>>();
  rationalSimplexSolver_ = make_shared<SimplexSolver<rational_t>>();
}


void
Gui::MainWindow::setupNumericSolversControllers()
{
  realSimplexSolverController_ = SimplexSolverController<real_t>(realSimplexSolver_);
  rationalSimplexSolverController_ =
    SimplexSolverController<rational_t>(rationalSimplexSolver_);
}


void Gui::MainWindow::updateNumericSolversData()
{
  switch (field_)
  {
    case Field::Real:
      {
        Matrix<real_t, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<real_t>(
            programTableModels_[int(ProgramModel::ObjFunc)]
          )
        );

        Matrix<real_t, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<real_t>(
            programTableModels_[int(ProgramModel::Constrs)]
          )
        );

        Matrix<real_t, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<real_t>(
            programTableModels_[int(ProgramModel::RHS)]
          )
        );

        LinearProgramData<real_t> linearProgramData(
          std::move(objFuncCoeffs),
          std::move(constrsCoeffs),
          std::move(constrsRHS)
        );

        realSimplexSolver_->setLinearProgramData(std::move(linearProgramData));

        break;
      }

    case Field::Rational:
      {
        Matrix<rational_t, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<rational_t>(
            programTableModels_[int(ProgramModel::ObjFunc)]
          )
        );

        Matrix<rational_t, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<rational_t>(
            programTableModels_[int(ProgramModel::Constrs)]
          )
        );

        Matrix<rational_t, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<rational_t>(
            programTableModels_[int(ProgramModel::RHS)]
          )
        );

        LinearProgramData<rational_t> linearProgramData(
          std::move(objFuncCoeffs),
          std::move(constrsCoeffs),
          std::move(constrsRHS)
        );

        rationalSimplexSolver_->setLinearProgramData(linearProgramData);

        break;
      }

    default:
      break;
  }
}


void
Gui::MainWindow::solveSimplex()
{
  updateNumericSolversData();

  ui->detailsTabWidget->setCurrentIndex(int(DetailsView::Simplex));
  ui->simplexMethodTab->setEnabled(true);

  enableCurrentSolutionSimplexView();
  ui->simplex_startPushButton->setEnabled(true);

  switch (field_)
  {
    case Field::Real:
      {
        const pair<SolutionType, optional<LinearProgramSolution<real_t>>>
        linearProgramSolution(realSimplexSolver_->solve());

        if (linearProgramSolution.second)
        {
          simplexTableModels_[int(SimplexModel::Solution)]->resize(
            1,
            (*linearProgramSolution.second).extremePoint.rows()
          );
          TableModelUtils::fill<real_t>(
            simplexTableModels_[int(SimplexModel::Solution)],
            (*linearProgramSolution.second).extremePoint.transpose()
          );

          TableModelUtils::fill(
            simplexTableModels_[int(SimplexModel::ObjectiveValue)],
            numericCast<QString, real_t>(
              (*linearProgramSolution.second).extremeValue
            )
          );

          LOG(
            "Solution: x* == {0}\nF* == {1}",
            (*linearProgramSolution.second).extremePoint,
            (*linearProgramSolution.second).extremeValue
          );
        }
        else
        {
          solutionErrorHandler(
            QStringLiteral(
              "This linear program can not be solved by the Simplex method."
            ),
            linearProgramSolution.first
          );
          goto error;
        }
        break;
      }

    case Field::Rational:
      {
        const pair<SolutionType, optional<LinearProgramSolution<rational_t>>>
        linearProgramSolution(rationalSimplexSolver_->solve());

        if (linearProgramSolution.second)
        {
          simplexTableModels_[int(SimplexModel::Solution)]->resize(
            1,
            (*linearProgramSolution.second).extremePoint.rows()
          );
          TableModelUtils::fill<rational_t>(
            simplexTableModels_[int(SimplexModel::Solution)],
            (*linearProgramSolution.second).extremePoint.transpose()
          );

          TableModelUtils::fill(
            simplexTableModels_[int(SimplexModel::ObjectiveValue)],
            numericCast<QString, rational_t>(
              (*linearProgramSolution.second).extremeValue
            )
          );

          LOG(
            "Solution: x* == {0}\nF* == {1}",
            (*linearProgramSolution.second).extremePoint,
            (*linearProgramSolution.second).extremeValue
          );
        }
        else
        {
          solutionErrorHandler(
            QStringLiteral(
              "This linear program can not be solved by the Simplex method."
            ),
            linearProgramSolution.first
          );
          goto error;
        }
        break;
      }

    error:
      enableCurrentSolutionSimplexView(false);
      enableStepByStepSimplexView(false);
      clearSimplexView();
      break;

    default:
      break;
  }
}


void
Gui::MainWindow::solveGraphical()
{
  ui->detailsTabWidget->setCurrentIndex(int(DetailsView::Graphical));
  ui->graphicalMethodTab->setEnabled(true);

  enableGraphicalSolutionView();

  switch (field_)
  {
    case Field::Real:
      {
        Matrix<real_t, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<real_t>(
            programTableModels_[int(ProgramModel::ObjFunc)]
          )
        );

        Matrix<real_t, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<real_t>(
            programTableModels_[int(ProgramModel::Constrs)]
          )
        );

        Matrix<real_t, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<real_t>(
            programTableModels_[int(ProgramModel::RHS)]
          )
        );

        LinearProgramData<real_t> linearProgramData(
          std::move(objFuncCoeffs),
          std::move(constrsCoeffs),
          std::move(constrsRHS)
        );

        GraphicalSolver2D<real_t> graphicalSolver2D(
          std::move(linearProgramData)
        );

        pair<SolutionType, optional<PlotData2D<real_t>>> plotData2D(
          graphicalSolver2D.solve()
        );

        if (plotData2D.second)
        {
          refreshGraphicalSolutionView(*plotData2D.second);
        }
        else
        {
          solutionErrorHandler(
            QStringLiteral(
              "This linear program can not be solved by"
              " the 2D graphical method."
            ),
            plotData2D.first
          );

          goto error;
        }

        break;
      }

    case Field::Rational:
      {
        Matrix<rational_t, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<rational_t>(
            programTableModels_[int(ProgramModel::ObjFunc)]
          )
        );

        Matrix<rational_t, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<rational_t>(
            programTableModels_[int(ProgramModel::Constrs)]
          )
        );

        Matrix<rational_t, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<rational_t>(
            programTableModels_[int(ProgramModel::RHS)]
          )
        );

        LinearProgramData<rational_t> linearProgramData(
          std::move(objFuncCoeffs),
          std::move(constrsCoeffs),
          std::move(constrsRHS)
        );

        GraphicalSolver2D<rational_t> graphicalSolver2D(
          std::move(linearProgramData)
        );

        pair<SolutionType, optional<PlotData2D<rational_t>>> plotData2D(
          graphicalSolver2D.solve()
        );

        if (plotData2D.second)
        {
          refreshGraphicalSolutionView(*plotData2D.second);
        }
        else
        {
          solutionErrorHandler(
            QStringLiteral(
              "This linear program cannot be solved by"
              " the 2D graphical method."
            ),
            plotData2D.first
          );

          goto error;
        }

        break;
      }

    error:
      enableGraphicalSolutionView(false);
      clearGraphicalSolutionView(ui->graphical_solutionPlotQCustomPlot);
      break;

    default:
      break;
  }
}


void
Gui::MainWindow::solutionErrorHandler(
  const QString& description, LinearProgramming::SolutionType type
)
{
  QString details;
  switch (type) {
    case SolutionType::Unknown:
      details = QStringLiteral(
        "The program could not be solved:\n"
        "Either the selected solver does not support this kind of"
        " linear programs\nor some unknown error was encountered..."
      );
      break;

    case SolutionType::Incomplete:
      details = QStringLiteral("Maximal iterations limit is exceeded.");
      break;

    case SolutionType::Infeasible:
      details = QStringLiteral(
        "This linear program is infeasible:\n"
        "The feasible region `Q' is empty set."
      );
      break;

    case SolutionType::Unbounded:
      details = QStringLiteral(
        "This linear program cannot be solved:\n"
        "The objective function `F' is unbounded"
        " over the feasible region `Q'."
      );
      break;

    case SolutionType::Optimal:
      details = QStringLiteral("The current solution `x*' is optimal.");
      break;

    default:
      details = QStringLiteral("Unknown error...");
      break;
  }

  QMessageBox::warning(
    this,
    QStringLiteral("Error..."),
    description + QStringLiteral("\n\n") + details
  );
}


void
Gui::MainWindow::pivotingErrorHandler(
  const QString& description, LinearProgramming::SolutionType type
)
{
  QString details;
  switch (type) {
    case SolutionType::Unknown:
      break;

    case SolutionType::Incomplete:
      break;

    case SolutionType::Infeasible:
      break;

    case SolutionType::Unbounded:
      details = QStringLiteral(
        "This linear program cannot be solved:\n"
        "The objective function `F' is unbounded"
        " over the feasible region `Q'."
      );
      break;

    case SolutionType::Optimal:
      details = QStringLiteral("The current solution `x*' is optimal.");
      break;

    default:
      details = QStringLiteral("Unknown error...");
      break;
  }

  QMessageBox::warning(
    this,
    QStringLiteral("Error..."),
    description + QStringLiteral("\n\n") + details
  );
}


void
Gui::MainWindow::openFile(const QString& filename)
{
  const ResultType result(loadDataFromFile(filename));
  switch (result)
  {
    case ResultType::Success:
      setWindowFilePath(filename);
      setDirty(false);
      break;

    case ResultType::Fail:
      QMessageBox::critical(
        this,
        QStringLiteral("File Reading Error"),
        QStringLiteral("Could not open the selected file.")
      );
      break;

    case ResultType::Nothing:
      QMessageBox::critical(
        this,
        QStringLiteral("File Reading Error"),
        QStringLiteral("Could not read contents of the selected file.")
      );
      break;

    default:
      break;
  }
}


void
Gui::MainWindow::saveFile(const QString& filename)
{
  const ResultType result(saveDataToFile(filename));
  switch (result)
  {
    case ResultType::Success:
      setWindowFilePath(filename);
      setDirty(false);
      break;

    case ResultType::Fail:
      QMessageBox::critical(
        this,
        QStringLiteral("File Writing Error"),
        QStringLiteral("Could not write contents to the selected location.")
      );

    case ResultType::Nothing:
      QMessageBox::critical(
        this,
        QStringLiteral("File Writing Error"),
        QStringLiteral("Could not save file to the selected location.")
      );

    default:
      break;
  }
}


Utils::ResultType
Gui::MainWindow::loadDataFromFile(const QString& filename)
{
  if (!filename.isEmpty())
  {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
      qWarning() << "MainWindow::loadDataFromFile:"
                    " couldn't open file at path" << filename;

      return ResultType::Nothing;
    }
    else
    {
      const QByteArray byteArray(file.readAll());
      if (byteArray.count() > 0)
      {
        const QJsonDocument jsonDocument(QJsonDocument::fromJson(byteArray));
        TableModelCollection tableModels;
        const ResultType res(tableModels.read(jsonDocument.object()));

        file.flush();
        file.close();

        if (res == ResultType::Success)
        {
          if (tableModels.count() == ProgramModelsCount)
          {
            for (int i(0); i < ProgramModelsCount; ++i)
            {
              //Transfer contents...
              (*programTableModels_[i]) = std::move(tableModels[i]);
              //...and update ownerhsip
              (*programTableModels_[i]).setParent(this);
            }
            assignTableModelsHeaders();

            //Update `N'
            ui->program_varsSpinBox->setValue(
              programTableModels_[int(ProgramModel::Constrs)]->columnCount()
            );

            //Update `M'
            ui->program_constrsSpinBox->setValue(
              programTableModels_[int(ProgramModel::Constrs)]->rowCount()
            );

            //Update `field_'
            field_ = tableModels.field();
            switch (field_)
            {
              case Field::Real:
                ui->program_realRadioButton->setChecked(true);
                break;

              case Field::Rational:
                ui->program_rationalRadioButton->setChecked(true);
                break;

              default:
                qWarning() << "MainWindow::loadData: unknown value of `field_'";
                return ResultType::Fail;
            }

            return ResultType::Success;
          }
          else
          {
            return ResultType::Nothing;
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

        return ResultType::Nothing;
      }
    }
  }
  else
  {
    return ResultType::Nothing;
  }
}


Utils::ResultType
Gui::MainWindow::saveDataToFile(const QString& filename)
{
  if (!filename.isEmpty())
  {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
      qWarning() << "MainWindow::saveDataToFile: couldn't open file"
                    " at path" << filename;

      return ResultType::Nothing;
    }
    else
    {
      QJsonObject jsonObject;
      //TODO: ~? Use pointers here.
      TableModelCollection tableModels(
        QVector<StringTableModel>{
          (*programTableModels_[int(ProgramModel::ObjFunc)]),
          (*programTableModels_[int(ProgramModel::Constrs)]),
          (*programTableModels_[int(ProgramModel::RHS)])
        },
        field_
      );
      const ResultType res(tableModels.write(jsonObject));

      if (res == ResultType::Success)
      {
        const QJsonDocument jsonDocument(jsonObject);

        const qint64 bytesCount(
          file.write(jsonDocument.toJson(QJsonDocument::Indented))
        );
        file.flush();
        file.close();

        if (bytesCount == -1)
        {
          return ResultType::Fail;
        }
        else
        {
          return ResultType::Success;
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
    return ResultType::Nothing;
  }
}


void
Gui::MainWindow::loadSettings()
{
  QSettings settings(
    QSettings::IniFormat, QSettings::UserScope, OrgName, Name
  );

  settings.beginGroup(QStringLiteral("mainWindow"));
  restoreGeometry(
    settings.value(QStringLiteral("geometry")).toByteArray()
  );
  restoreState(
    settings.value(QStringLiteral("windowState")).toByteArray()
  );
  settings.endGroup();
}


void
Gui::MainWindow::saveSettings()
{
  QSettings settings(
    QSettings::IniFormat, QSettings::UserScope, OrgName, Name
  );

  settings.beginGroup(QStringLiteral("mainWindow"));
  settings.setValue(QStringLiteral("geometry"), saveGeometry());
  settings.setValue(QStringLiteral("windowState"), saveState());
  settings.endGroup();
}

#pragma endregion // private methods


#pragma region public slots

void
Gui::MainWindow::on_graphical_solutionPlotQCustomPlot_selectionChangedByUser()
{
  if (
    ui->graphical_solutionPlotQCustomPlot->xAxis->selectedParts().
      testFlag(QCPAxis::spAxis) ||
    ui->graphical_solutionPlotQCustomPlot->xAxis->selectedParts().
      testFlag(QCPAxis::spTickLabels) ||
    ui->graphical_solutionPlotQCustomPlot->xAxis2->selectedParts().
      testFlag(QCPAxis::spAxis) ||
    ui->graphical_solutionPlotQCustomPlot->xAxis2->selectedParts().
      testFlag(QCPAxis::spTickLabels)
  )
  {
    ui->graphical_solutionPlotQCustomPlot->xAxis2->setSelectedParts(
      QCPAxis::spAxis | QCPAxis::spTickLabels
    );
    ui->graphical_solutionPlotQCustomPlot->xAxis->setSelectedParts(
      QCPAxis::spAxis | QCPAxis::spTickLabels
    );
  }

  if (
    ui->graphical_solutionPlotQCustomPlot->yAxis->selectedParts().
      testFlag(QCPAxis::spAxis) ||
    ui->graphical_solutionPlotQCustomPlot->yAxis->selectedParts().
      testFlag(QCPAxis::spTickLabels) ||
    ui->graphical_solutionPlotQCustomPlot->yAxis2->selectedParts().
      testFlag(QCPAxis::spAxis) ||
    ui->graphical_solutionPlotQCustomPlot->yAxis2->selectedParts().
      testFlag(QCPAxis::spTickLabels)
  )
  {
    ui->graphical_solutionPlotQCustomPlot->yAxis2->setSelectedParts(
      QCPAxis::spAxis | QCPAxis::spTickLabels
    );
    ui->graphical_solutionPlotQCustomPlot->yAxis->setSelectedParts(
      QCPAxis::spAxis | QCPAxis::spTickLabels
    );
  }

  for (
    int i(0); i < ui->graphical_solutionPlotQCustomPlot->plottableCount(); ++i
  )
  {
    QCPAbstractPlottable* const plottable =
      ui->graphical_solutionPlotQCustomPlot->plottable(i);
    QCPPlottableLegendItem* const item =
      ui->graphical_solutionPlotQCustomPlot->legend->itemWithPlottable(
        plottable
      );
    if (item->selected() || plottable->selected())
    {
      item->setSelected(true);
      plottable->setSelected(true);
    }
  }
}


void
Gui::MainWindow::on_graphical_solutionPlotQCustomPlot_mousePress(QMouseEvent* ev)
{
  if (
    ui->graphical_solutionPlotQCustomPlot->xAxis->selectedParts().
    testFlag(QCPAxis::spAxis)
  )
  {
    ui->graphical_solutionPlotQCustomPlot->axisRect()->setRangeDrag(
      ui->graphical_solutionPlotQCustomPlot->xAxis->orientation()
    );
  }
  else
  {
    if (
      ui->graphical_solutionPlotQCustomPlot->yAxis->selectedParts().
      testFlag(QCPAxis::spAxis)
    )
    {
      ui->graphical_solutionPlotQCustomPlot->axisRect()->setRangeDrag(
        ui->graphical_solutionPlotQCustomPlot->yAxis->orientation()
      );
    }
    else
    {
      ui->graphical_solutionPlotQCustomPlot->axisRect()->setRangeDrag(
        Qt::Horizontal | Qt::Vertical
      );
    }
  }
}


void
Gui::MainWindow::on_graphical_solutionPlotQCustomPlot_mouseWheel(QWheelEvent* ev)
{
  if (
    ui->graphical_solutionPlotQCustomPlot->xAxis->selectedParts().
    testFlag(QCPAxis::spAxis)
  )
  {
    ui->graphical_solutionPlotQCustomPlot->axisRect()->setRangeZoom(
      ui->graphical_solutionPlotQCustomPlot->xAxis->orientation()
    );
  }
  else
  {
    if (
      ui->graphical_solutionPlotQCustomPlot->yAxis->selectedParts().
      testFlag(QCPAxis::spAxis)
    )
    {
      ui->graphical_solutionPlotQCustomPlot->axisRect()->setRangeZoom(
        ui->graphical_solutionPlotQCustomPlot->yAxis->orientation()
      );
    }
    else
    {
      ui->graphical_solutionPlotQCustomPlot->axisRect()->setRangeZoom(
        Qt::Horizontal | Qt::Vertical
      );
    }
  }
}


void
Gui::MainWindow::on_graphical_solutionPlotQCustomPlot_mouseMove(QMouseEvent* ev)
{
  const double x(
    ui->graphical_solutionPlotQCustomPlot->xAxis->pixelToCoord(
      ev->pos().x()
    )
  );
  const double y(
    ui->graphical_solutionPlotQCustomPlot->yAxis->pixelToCoord(
      ev->pos().y()
    )
  );

  ui->graphical_solutionPlotQCustomPlot->setToolTip(
    QString("(%1; %2)").arg(x).arg(y)
  );
}


void
Gui::MainWindow::on_anyProgramModel_dataChanged(
  const QModelIndex& topLeft,
  const QModelIndex& bottomRight, const QVector<int>& roles)
{
  if (isLoaded_)
  {
    setDirty();

    ui->program_objFuncCoeffsTableView->resizeColumnsToContents();
    ui->program_constrsCoeffsTableView->resizeColumnsToContents();
    ui->program_constrsRHSTableView->resizeColumnsToContents();
  }
}


void
Gui::MainWindow::on_anyProgramModel_dimensionsChanged(
  const QModelIndex& parent, int start, int end
)
{
  if (isLoaded_)
  {
    setDirty();
  }
}


void
Gui::MainWindow::on_anyProgramModel_modelReset()
{
  if (isLoaded_)
  {
    setDirty();

    ui->program_objFuncCoeffsTableView->resizeColumnsToContents();
    ui->program_constrsCoeffsTableView->resizeColumnsToContents();
    ui->program_constrsRHSTableView->resizeColumnsToContents();
  }
}


void
Gui::MainWindow::on_program_constrsSpinBox_valueChanged(int arg1)
{
  if (arg1 >= MinConstraints && arg1 <= MaxConstraints)
  {
    if (ui->program_varsSpinBox->value() <= arg1) //If (N <= M')
    {
      ui->program_varsSpinBox->setValue(arg1); //N := M'
    }

    const int oldRowsCount(
      programTableModels_[int(ProgramModel::Constrs)]->rowCount()
    );
    const int oldColsCount(
      programTableModels_[int(ProgramModel::Constrs)]->columnCount()
    );

    programTableModels_[int(ProgramModel::Constrs)]->resize(arg1, 0);
    programTableModels_[int(ProgramModel::Constrs)]->clear(
      oldRowsCount, 0, arg1, oldColsCount, QStringLiteral("0")
    );

    programTableModels_[int(ProgramModel::RHS)]->resize(arg1, 0);
    programTableModels_[int(ProgramModel::RHS)]->clear(
      oldRowsCount, 0, arg1, 1, QStringLiteral("0")
    );
  }
}


void
Gui::MainWindow::on_program_varsSpinBox_valueChanged(int arg1)
{
  if (arg1 >= MinVariables && arg1 <= MaxVariables)
  {
    if (ui->program_constrsSpinBox->value() > arg1) //If (M > N')
    {
      ui->program_constrsSpinBox->setValue(arg1); //M := N'
    }

    const int oldColsCount(
      programTableModels_[int(ProgramModel::Constrs)]->columnCount()
    );
    const int oldRowsCount(
      programTableModels_[int(ProgramModel::Constrs)]->rowCount()
    );

    programTableModels_[int(ProgramModel::Constrs)]->resize(0, arg1);
    programTableModels_[int(ProgramModel::Constrs)]->clear(
      0, oldColsCount, oldRowsCount, arg1, QStringLiteral("0")
    );

    programTableModels_[int(ProgramModel::ObjFunc)]->resize(0, arg1);
    programTableModels_[int(ProgramModel::ObjFunc)]->clear(
      0, oldColsCount, 1, arg1, QStringLiteral("0")
    );
  }
}


void
Gui::MainWindow::on_program_realRadioButton_toggled(bool checked)
{
  if (checked)
  {
    setField(Field::Real);
  }
}


void
Gui::MainWindow::on_program_rationalRadioButton_toggled(bool checked)
{
  if (checked)
  {
    setField(Field::Rational);
  }
}


void
Gui::MainWindow::on_control_solveSimplexPushButton_clicked()
{
  solveSimplex();
}


void
Gui::MainWindow::on_control_solveGraphicalPushButton_clicked()
{
  solveGraphical();
}


void
Gui::MainWindow::on_control_clearPushButton_clicked()
{
  clearProgramView();
}


[[deprecated("To be removed in release version!")]]
void
Gui::MainWindow::on_control_testPushButton_clicked()
{
  {
    loadDataFromFile(QStringLiteral("data7.json"));

    setDirty(false);
  }
}


void
Gui::MainWindow::on_simplex_startPushButton_clicked()
{
  //NOTE: `Start' button is only accessible by clicking `Solve' button so
  //we don't need to force updating of program data
//  updateNumericSolversData();

  enableCurrentSolutionSimplexView();
  enableStepByStepSimplexView();

  switch (field_)
  {
    case Field::Real:
      realSimplexSolverController_.reset();
      realSimplexSolverController_.start();
      refreshSimplexView();
      break;

    case Field::Rational:
      rationalSimplexSolverController_.reset();
      rationalSimplexSolverController_.start();
      refreshSimplexView();
      break;

    default:
      break;
  }
}


void
Gui::MainWindow::on_simplex_stepBackPushButton_clicked()
{
  switch (field_)
  {
    case Field::Real:
      if (realSimplexSolverController_.hasPrevious())
      {
        realSimplexSolverController_.previous();
      }
      refreshSimplexView();
      break;

    case Field::Rational:
      if (rationalSimplexSolverController_.hasPrevious())
      {
        rationalSimplexSolverController_.previous();
      }
      refreshSimplexView();
      break;

    default:
      break;
  }
}


void
Gui::MainWindow::on_simplex_stepForwardPushButton_clicked()
{
  optional<pair<DenseIndex, DenseIndex>> pivotIdx;

  if (ui->simplex_manualPivotCheckBox->isChecked())
  {
    const QModelIndex idx(
      ui->simplex_simplexTableauTableView->selectionModel()->currentIndex()
    );

    if (idx.isValid())
    {
      pivotIdx = make_pair(idx.row(), idx.column());
    }
  }

  switch (field_)
  {
    case Field::Real:
      if (realSimplexSolverController_.hasNext())
      {
        realSimplexSolverController_.next(pivotIdx);
      }
      refreshSimplexView();
      break;

    case Field::Rational:
      if (rationalSimplexSolverController_.hasNext())
      {
        rationalSimplexSolverController_.next(pivotIdx);
      }
      refreshSimplexView();
      break;

    default:
      break;
  }
}


void
Gui::MainWindow::on_simplex_manualPivotCheckBox_toggled(bool checked)
{
  if (!checked)
  {
    ui->simplex_simplexTableauTableView->selectionModel()->clearSelection();
    ui->simplex_simplexTableauTableView->selectionModel()->clearCurrentIndex();
  }
}


void
Gui::MainWindow::on_simplex_pivotHintPushButton_clicked()
{
  switch (field_)
  {
    case Field::Real:
      if (!realSimplexSolverController_.isEmpty())
      {
        const pair<SolutionType, MaybeIndex2D> pivot(
          realSimplexSolverController_.pivot()
        );

        if (pivot.second)
        {
          //TODO: ~ Depending on the specified command, the index can also
          //become part of the current selection.
          const QModelIndex idx(
            simplexTableModels_[int(SimplexModel::Tableau)]->index(
             (*pivot.second).first, (*pivot.second).second
            )
          );

          ui->simplex_simplexTableauTableView->selectionModel()->
            setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);

          ui->simplex_simplexTableauTableView->selectionModel()->
            select(idx, QItemSelectionModel::ClearAndSelect);
        }
        else
        {
          pivotingErrorHandler(
            QStringLiteral("No pivot element can be chosen for"
                           " the current Simplex tableau."),
            pivot.first
          );
        }
      }
      break;

    case Field::Rational:
      if (!rationalSimplexSolverController_.isEmpty())
      {
        const pair<SolutionType, MaybeIndex2D> pivot(
          rationalSimplexSolverController_.pivot()
        );

        if (pivot.second)
        {
          //TODO: ~ Depending on the specified command, the index can also
          //become part of the current selection.
          const QModelIndex idx(
            simplexTableModels_[int(SimplexModel::Tableau)]->index(
             (*pivot.second).first, (*pivot.second).second
            )
          );

          ui->simplex_simplexTableauTableView->selectionModel()->
            setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);

          ui->simplex_simplexTableauTableView->selectionModel()->
            select(idx, QItemSelectionModel::ClearAndSelect);
        }
        else
        {
          pivotingErrorHandler(
            QStringLiteral("No pivot element can be chosen for"
                           " the current Simplex tableau."),
            pivot.first
          );
        }
      }
      break;

    default:
      break;
  }
}


void
Gui::MainWindow::on_action_Open_triggered()
{
  //TODO: ~ Check for unsaved changes.
  const QString filename(
    QFileDialog::getOpenFileName(
      this,
      QStringLiteral("Open Linear Program..."),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
        "/linearProgram.json",
      QStringLiteral("JSON file (*.json);;Plain text document (*.txt)"),
      0,
      QFileDialog::DontUseNativeDialog
    )
  );

  if (!filename.isEmpty())
  {
    openFile(filename);
  }
}


void
Gui::MainWindow::on_action_Save_as_triggered()
{
  const QString filename(
    QFileDialog::getSaveFileName(
      this,
      QStringLiteral("Save Linear Program As"),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
        QString("/linearProgram@%1.json").arg(
          QDateTime::currentDateTime().toString(QStringLiteral("dMMMyy_h-m-s"))
        ),
      QStringLiteral("JSON file (*.json);;Plain text document (*.txt)"),
      0,
      QFileDialog::DontUseNativeDialog
    )
  );

  if (!filename.isEmpty())
  {
    saveFile(filename);
  }
}


void
Gui::MainWindow::on_action_Quit_triggered()
{
  QApplication::quit();
}


void
Gui::MainWindow::on_action_Undo_triggered()
{
  //TODO: ~ Undo action.
}


void
Gui::MainWindow::on_action_Redo_triggered()
{
  //TODO: ~ Redo action.
}


void
Gui::MainWindow::on_action_Clear_triggered()
{
  clearProgramView();
}


void
Gui::MainWindow::on_action_Fill_w_random_numbers_triggered()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    TableModelUtils::fill(
      programTableModels_[i],
      TableModelUtils::FillMethod::Random
    );
  }
}


void
Gui::MainWindow::on_action_Zoom_in_triggered()
{
  //TODO: ~ Connect to QCP.
}


void
Gui::MainWindow::on_action_Zoom_out_triggered()
{
  //TODO: ~ Connect to QCP.
}


void
Gui::MainWindow::on_action_Zoom_reset_triggered()
{
//  ui->graphical_solutionPlotQCustomPlot->xAxis->setRange(
//    1.,
//    ui->graphical_solutionPlotQCustomPlot->xAxis->range().size(),
//    Qt::AlignCenter
//  );

//  ui->graphical_solutionPlotQCustomPlot->yAxis->setRange(
//    1.,
//    ui->graphical_solutionPlotQCustomPlot->yAxis->range().size(),
//    Qt::AlignCenter
//  );

//  ui->graphical_solutionPlotQCustomPlot->replot();
  //TODO: ~ Connect to QCP.
}


void
Gui::MainWindow::on_action_Solve_triggered()
{
  solveSimplex();
}


void
Gui::MainWindow::on_action_Plot_triggered()
{
  solveGraphical();
}


void
Gui::MainWindow::on_action_How_to_triggered()
{
  //TODO: ~ Add how-to guide.
}


/**
 * @brief Gui::MainWindow::on_action_About_triggered
 * NOTE: See
 *   http://www.gnome.org
 *   http://wiki.gnome.org/Design
 *   http://git.gnome.org/browse/adwaita-icon-theme
 *   http://ftp.gnome.org/pub/gnome/sources/adwaita-icon-theme/3.16
 *   http://github.com/GNOME/adwaita-icon-theme'
 */
void
Gui::MainWindow::on_action_About_triggered()
{
  QMessageBox::about(
    this,
    QStringLiteral("About Application"),
    QStringLiteral(
      "<h3><b>Linear Programming ver. "
      LP_APP_VERSION
      " (build date: "
      __DATE__
      " "
      __TIME__
      ")</b></h3>"
      "Application demonstrating some methods of solving some types of "
      "linear optimization problems."
      "<hr>"
      "This software uses:"
      "<ul>"
      "<li><a href=\"http://www.qt.io\">Qt (ver. "
      LP_QT_VERSION
      ")</a></li>"
      "<li><a href=\"http://eigen.tuxfamily.org\">Eigen (ver. "
      LP_EIGEN_VERSION
      ")</a></li>"
      "<li><a href=\"http://www.qcustomplot.com\">"
      "QCustomPlot (ver. "
      LP_QCUSTOMPLOT_VERSION
      ")</a></li>"
      "<li><a href=\"http://www.boost.org\">Boost (ver. "
      LP_BOOST_VERSION
      ")</a></li>"
      "<li><a href=\"http://cppformat.github.io\">"
      "C++ Format (ver. "
      LP_CPPFORMAT_VERSION
      ")</a></li>"
      "<li><a href=\"http://louisdx.github.io/cxx-prettyprint\">"
      "cxx-prettyprint (ver. "
      LP_CXXPRETTYPRINT_VERSION
      ")</a></li>"
      "<li><a href=\"http://git.gnome.org/browse/adwaita-icon-theme\">"
      "Adwaita Icon Theme (ver. "
      LP_ADWAITA_VERSION
      ")</a> "
      "by the <a href=\"http://www.gnome.org\">GNOME Project</a></li>"
      "</ul>"
      "<hr>"
      "Copyright (C) 2015  Alexey Gorishny"
      "<br><br>"
      "This program is free software: you can redistribute it and/or modify "
      "it under the terms of the GNU General Public License as published by "
      "the Free Software Foundation, either version 3 of the License, or "
      "(at your option) any later version."
      "<br><br>"
      "This program is distributed in the hope that it will be useful, "
      "but WITHOUT ANY WARRANTY; without even the implied warranty of "
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
      "GNU General Public License for more details."
      "<br><br>"
      "You should have received a copy of the GNU General Public License "
      "along with this program.  If not, see "
      "<a href=\"https://www.gnu.org/licenses\">"
      "Licenses - GNU Project - Free Software Foundation</a>."
    )
  );
}


void
Gui::MainWindow::on_action_About_Qt_triggered()
{
  QApplication::aboutQt();
}

#pragma endregion // public slots
