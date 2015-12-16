#include "mainwindow.hxx"

#include "ui_mainwindow.h"

#include <cstdlib>
#include <cmath>
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
#include <QList>
#include <QMessageBox>
#include <QMimeData>
#include <QModelIndex>
#include <QPen>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QVector>

#include "boost/optional.hpp"
#include "eigen3/Eigen/Core"
#include "qcustomplot/qcustomplot.h"

#include "numericstyleditemdelegate.hxx"
#include "simpletablemodel.hxx"
#include "tablemodelstorage.hxx"
#include "tablemodelutils.hxx"
#include "../lp/dantzignumericsolver.hxx"
#include "../lp/dantzignumericsolvercontroller.hxx"
#include "../lp/graphicalsolver2d.hxx"
#include "../lp/linearprogrammingutils.hxx"
#include "../lp/linearprogramsolution.hxx"
#include "../lp/plotdatareal2d.hxx"
#include "../lp/solutiontype.hxx"
#include "../math/mathutils.hxx"
#include "../misc/dataconvertors.hxx"
#include "../misc/utils.hxx"
#include "../config.hxx"


namespace GUI
{
  using boost::optional;
  using namespace Config::AppGlobal;
  using namespace Config::GUI;
  using DataConvertors::numericCast;
  using Eigen::DenseIndex;
  using Eigen::Dynamic;
  using Eigen::Matrix;
  using LinearProgramming::GraphicalSolver2D;
  using LinearProgramming::LinearProgramData;
  using LinearProgramming::LinearProgramSolution;
  using LinearProgramming::MaybeIndex2D;
  using LinearProgramming::SimplexTableau;
  using LinearProgrammingUtils::blerp;
  using LinearProgrammingUtils::perp;
  using std::make_shared;
  using std::move;
  using std::pair;
  using std::vector;
}


#pragma region ctors/dtors

GUI::MainWindow::MainWindow(QWidget* parent) :
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

  _isLoaded = true;
}


GUI::MainWindow::~MainWindow()
{
  saveSettings();

  destroySimplexView();
  destroyProgramView();

  delete ui;
}

#pragma endregion


#pragma region event handlers

void
GUI::MainWindow::closeEvent(QCloseEvent* closeEvent)
{
  //TODO: ~ Check if the content is _really_ modified.
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
        emit on_action_Save_as_triggered();

        if (!isWindowModified())
        {
          closeEvent->accept();

          break;
        }
        else
        {
          goto begin;
        }

      case QMessageBox::Discard:
        closeEvent->accept();
        break;

      case QMessageBox::Cancel:
        closeEvent->ignore();
        break;

      default:
        break;
    }
  }
  else
  {
    closeEvent->accept();
  }
}


void
GUI::MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
  if (event->mimeData()->hasUrls())
  {
    event->acceptProposedAction();
  }
  else
  {
    event->ignore();
  }
}


void
GUI::MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
  if (event->mimeData()->hasUrls())
  {
    event->acceptProposedAction();
  }
  else
  {
    event->ignore();
  }
}


void
GUI::MainWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
  event->accept();
}


void
GUI::MainWindow::dropEvent(QDropEvent* event)
{
  const QMimeData* const mimeData = event->mimeData();

  if (mimeData->hasUrls())
  {
    const QList<QUrl> urls(mimeData->urls());
    for (int i(0); i < urls.count() && i < 1; ++i)
    {
      const QUrl url(urls.at(i));
      const QFileInfo fileInfo(url.toLocalFile());
      if (fileInfo.isFile())
      {
        const QString suffix(fileInfo.completeSuffix());
        if (
          (suffix.compare(QStringLiteral("json"), Qt::CaseInsensitive) == 0) ||
          (suffix.compare(QStringLiteral("txt"), Qt::CaseInsensitive) == 0)
        )
        {
          openFileHandler(fileInfo.canonicalFilePath());

          event->acceptProposedAction();
        }
        else
        {
          event->ignore();
        }
      }
      else
      {
        event->ignore();
      }
    }
  }
  else
  {
    event->ignore();
  }
}

#pragma endregion


#pragma region private methods

void
GUI::MainWindow::setupDefaults()
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
GUI::MainWindow::setupSignals()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    connect(
      _programTableModels[i],
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
      _programTableModels[i],
      SIGNAL(rowsInserted(const QModelIndex&, int, int)),
      this,
      SLOT(on_anyProgramModel_dimensionsChanged(const QModelIndex&, int, int))
    );

    connect(
      _programTableModels[i],
      SIGNAL(rowsRemoved(const QModelIndex&, int, int)),
      this,
      SLOT(on_anyProgramModel_dimensionsChanged(const QModelIndex&, int, int))
    );

    connect(
      _programTableModels[i],
      SIGNAL(columnsInserted(const QModelIndex&, int, int)),
      this,
      SLOT(on_anyProgramModel_dimensionsChanged(const QModelIndex&, int, int))
    );

    connect(
      _programTableModels[i],
      SIGNAL(columnsRemoved(const QModelIndex&, int, int)),
      this,
      SLOT(on_anyProgramModel_dimensionsChanged(const QModelIndex&, int, int))
    );

    connect(
      _programTableModels[i], SIGNAL(modelReset()),
      this, SLOT(on_anyProgramModel_modelReset())
    );
  }
}


void
GUI::MainWindow::setupProgramView()
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

  _programTableModels[int(ProgramModel::ObjFunc)]->insertRow(0);
  _programTableModels[int(ProgramModel::ObjFunc)]->insertColumns(
    0, DefaultVariables
  );
  ui->program_objFuncCoeffsTableView->setModel(
    _programTableModels[int(ProgramModel::ObjFunc)]
  );

  _programTableModels[int(ProgramModel::Constrs)]->insertRows(
    0, DefaultConstraints
  );
  _programTableModels[int(ProgramModel::Constrs)]->insertColumns(
    0, DefaultVariables
  );
  ui->program_constrsCoeffsTableView->setModel(
    _programTableModels[int(ProgramModel::Constrs)]
  );

  _programTableModels[int(ProgramModel::RHS)]->insertRows(
    0, DefaultConstraints
  );
  _programTableModels[int(ProgramModel::RHS)]->insertColumn(0);
  ui->program_constrsRHSTableView->setModel(
    _programTableModels[int(ProgramModel::RHS)]
  );
}


void
GUI::MainWindow::clearProgramView()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    _programTableModels[i]->clear(QStringLiteral("0"));
  }
}


void
GUI::MainWindow::destroyProgramView()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    delete _realNumericDelegates[i];
    delete _ratNumericDelegates[i];
    delete _programTableModels[i];
  }
}


void
GUI::MainWindow::setupGraphicalSolutionView(QCustomPlot* const customPlot)
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

  customPlot->xAxis->setLabel(QString("x%1").arg(1)); //TODO: ! Use proper names here
  customPlot->yAxis->setLabel(QString("x%1").arg(2)); //TODO: ! Use proper names here

  customPlot->setBackgroundScaled(true);
  customPlot->setBackgroundScaledMode(Qt::IgnoreAspectRatio);

  connect(
    customPlot, SIGNAL(selectionChangedByUser()),
    this, SLOT(on_graphical_solutionPlotQCustomPlot_selectionChangedByUser())
  );

  connect(
    customPlot, SIGNAL(mousePress(QMouseEvent*)),
    this, SLOT(on_graphical_solutionPlotQCustomPlot_mousePress())
  );

  connect(
    customPlot, SIGNAL(mouseWheel(QWheelEvent*)),
    this,
    SLOT(on_graphical_solutionPlotQCustomPlot_mouseWheel())
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
GUI::MainWindow::enableGraphicalSolutionView(bool enabled)
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
GUI::MainWindow::clearGraphicalSolutionView(QCustomPlot* const customPlot)
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


void
GUI::MainWindow::refreshGraphicalSolutionView(const PlotDataReal2D& plotData) {
  QCustomPlot* const customPlot = ui->graphical_solutionPlotQCustomPlot;

  setupGraphicalSolutionView(customPlot);

  const QFont normalFont(font().family(), FontSize);
  const QFont boldFont(font().family(), FontSize, QFont::Bold);

  const QVector<QColor> colors{
    Qt::red, Qt::green, Qt::blue,
    Qt::cyan, Qt::magenta, Qt::yellow
  };

  const int colorsCount(colors.count());

  const vector<Matrix<Real, 2, 1>> vertices(
    plotData.feasibleRegionExtremePoints.cbegin(),
    plotData.feasibleRegionExtremePoints.cend()
  );

  const size_t verticesCount(plotData.feasibleRegionExtremePoints.size());

  //Add colormap for obj. func. values...
  const int xSize(
    ColorMapResolution *
    (
      plotData.feasibleRegionBoundingBox(0, 1) -
      plotData.feasibleRegionBoundingBox(0, 0) + .5
    )
  );
  const int ySize(
    ColorMapResolution *
    (
      plotData.feasibleRegionBoundingBox(1, 1) -
      plotData.feasibleRegionBoundingBox(1, 0) + .5
    )
  );

  LOG("{} {}", xSize, ySize);

  QCPColorMap* const colorMap =
    new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
  colorMap->data()->setSize(xSize, ySize);
  colorMap->data()->setRange(
    QCPRange(
      plotData.feasibleRegionBoundingBox(0, 0),
      plotData.feasibleRegionBoundingBox(0, 1)
    ),
    QCPRange(
      plotData.feasibleRegionBoundingBox(1, 0),
      plotData.feasibleRegionBoundingBox(1, 1)
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

    double x, y;
    for (int xIdx(0); xIdx < xSize; ++xIdx)
    {
      for (int yIdx(0); yIdx < ySize; ++yIdx)
      {
        colorMap->data()->cellToCoord(xIdx, yIdx, &x, &y);
        const double z(blerpOverBoundingBox(x, y));
        colorMap->data()->setCell(xIdx, yIdx, z);
      }
    }
  }

//  {
//    int xIdx, yIdx;
//    for (size_t i(0); i < verticesCount; ++i)
//    {
//      const PointReal2D point(vertices[i]);

//      colormap->data()->coordToCell(
//        point.x, point.y, &xIdx, &yIdx
//      );
//      colormap->data()->setCell(xIdx, yIdx, point.second);
//    }
//  }

  //w/ a corresponding color scale
  QCPColorScale* const colorScale = new QCPColorScale(customPlot);
  colorScale->setType(QCPAxis::atRight);
  colorMap->setColorScale(colorScale);
  colorScale->axis()->setLabel("F^");
  customPlot->plotLayout()->insertColumn(1);
  customPlot->plotLayout()->addElement(0, 1, colorScale);

  QCPColorGradient colorGradient;
  colorGradient.setColorStops(ColorMap_PuCy);
  colorGradient.setLevelCount(ColorMapLevelsCount);
  colorGradient.setColorInterpolation(QCPColorGradient::ciHSV);
  colorMap->setGradient(colorGradient);

  colorMap->setInterpolate(true);
  colorMap->setTightBoundary(true);
  colorMap->rescaleDataRange(true);
  colorMap->setName(QString("F^"));
  colorMap->updateLegendIcon();
  colorMap->setSelectable(true);
  customPlot->addPlottable(colorMap);

  //Make sure the axis rect and color scale synchronize their
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

  //Add feasible region `U' as `QCPCurve'
  QCPCurve* const feasibleRegionCurve =
    new QCPCurve(customPlot->xAxis, customPlot->yAxis);
  QVector<Real> x1(verticesCount + 1), y1(verticesCount + 1);
  for (size_t i(0); i < verticesCount; ++i)
  {
    x1[i] = vertices[i].x();
    y1[i] = vertices[i].y();
  }
  x1[verticesCount] = vertices.front().x();
  y1[verticesCount] = vertices.front().y();
  feasibleRegionCurve->setData(x1, y1);
  feasibleRegionCurve->setPen(QPen(Qt::transparent));
  feasibleRegionCurve->setBrush(QBrush(QColor(31, 31, 31, 31)));
  feasibleRegionCurve->setSelectedPen(QPen(Qt::transparent));
  feasibleRegionCurve->setSelectedBrush(QBrush(QColor(32, 32, 32, 63)));
  feasibleRegionCurve->setScatterStyle(
    QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(255, 0, 0), 8)
  );
  feasibleRegionCurve->setName(QStringLiteral("U"));
  feasibleRegionCurve->setSelectable(true);
  customPlot->addPlottable(feasibleRegionCurve);

  //Add constraints
  for (size_t i(0); i < verticesCount; ++i) {
    //Add constraint as infinite line
    QCPItemStraightLine* const constraintAsContiniousLine =
      new QCPItemStraightLine(customPlot);
    constraintAsContiniousLine->setPen(QPen(Qt::gray));
    constraintAsContiniousLine->point1->setCoords(
      QPointF(vertices[i].x(), vertices[i].y())
    );
    constraintAsContiniousLine->point2->setCoords(
      QPointF(
        vertices[(i + 1) % verticesCount].x(),
        vertices[(i + 1) % verticesCount].y()
      )
    );
    constraintAsContiniousLine->setSelectable(false);
    customPlot->addItem(constraintAsContiniousLine);

    //Constraint as a feasible region edge
    QVector<Real> x(2), y(2); //add edge data
    x[0] = vertices[i].x();
    x[1] = vertices[(i + 1) % verticesCount].x();
    y[0] = vertices[i].y();
    y[1] = vertices[(i + 1) % verticesCount].y();
    customPlot->addGraph(); //create graph and assign data to it
    customPlot->graph(i)->setData(x, y);
    auto defaultColor(colors[i % colorsCount]); //set graph look
//    auto brushColor(defaultColor); brushColor.setAlphaF(.25);
//    auto selectedBrushColor(brushColor); selectedBrushColor.setAlphaF(.125);
    customPlot->graph(i)->setPen(QPen(defaultColor));
//    customPlot->graph(i)->setBrush(QBrush(brushColor));
    customPlot->graph(i)->setSelectedPen(
      QPen(QBrush(defaultColor), SelectedPenWidth)
    );
//    customPlot->graph(i)->setSelectedBrush(QBrush(selectedBrushColor));
    customPlot->graph(i)->setName(QString("Eq. (%1)").arg(i + 1));
    customPlot->graph(i)->setSelectable(true);
  }

  //Add objective function level line...
  const Matrix<Real, 2, 1> levelLine(perp(plotData.gradientVector));
  QCPItemStraightLine* const objFuncPlotLine =
    new QCPItemStraightLine(customPlot);
  objFuncPlotLine->point1->setCoords(0., 0.);
  objFuncPlotLine->point2->setCoords(levelLine.x(), levelLine.y());
  objFuncPlotLine->setPen(QPen(Qt::darkBlue));
  objFuncPlotLine->setSelectedPen(
    QPen(QBrush(Qt::darkBlue), SelectedPenWidth)
  );
  objFuncPlotLine->setSelectable(true);
  customPlot->addItem(objFuncPlotLine);

  //HACK: Invisible level line (only a section) to obtain anchor for text label.
  QCPItemLine* const levelLineSection = new QCPItemLine(customPlot);
  levelLineSection->start->setCoords(0., 0.);
  levelLineSection->end->setCoords(levelLine.x(), levelLine.y());
  levelLineSection->setPen(QPen(Qt::transparent));
  levelLineSection->setSelectable(false);
  customPlot->addItem(levelLineSection);

  //...w/ label
  QCPItemText* const objFuncLevelLineText = new QCPItemText(customPlot);
  objFuncLevelLineText->setPositionAlignment(Qt::AlignTop | Qt::AlignRight);
  objFuncLevelLineText->position->setParentAnchor(levelLineSection->start);
//  objFuncLevelLineText->position->setType(QCPItemPosition::ptPlotCoords);
  objFuncLevelLineText->position->setCoords(-1., 0.);
//  objFuncLevelLineText->setRotation(levelLine.angle());
  objFuncLevelLineText->setText(QStringLiteral("F(⃗x) = const"));
  objFuncLevelLineText->setFont(boldFont);
  objFuncLevelLineText->setSelectable(false);
  customPlot->addItem(objFuncLevelLineText);

  //Add gradient vector...
  QCPItemLine* const objFuncGradientVectorArrow = new QCPItemLine(customPlot);
  objFuncGradientVectorArrow->start->setCoords(QPointF(0., 0.));
  objFuncGradientVectorArrow->end->setCoords(
    plotData.gradientVector.x(), plotData.gradientVector.y()
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
    QString("∇F(⃗x) = (%1; %2)").
    arg(plotData.gradientVector.x()).
    arg(plotData.gradientVector.y())
  );
  objFuncGradientVectorArrowText->setSelectable(false);
  customPlot->addItem(objFuncGradientVectorArrowText);

  //Add text label at the top:
  QCPItemText* const objectiveValueText = new QCPItemText(customPlot);
  objectiveValueText->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
  objectiveValueText->position->setType(QCPItemPosition::ptAxisRectRatio);
  objectiveValueText->position->setCoords(.33, .033);
  objectiveValueText->setFont(boldFont);
  objectiveValueText->setText(
    QString(" F* = F(%1, %2) = %3 ").
    arg(plotData.extremePoints.front().x()).
    arg(plotData.extremePoints.front().y()).
    arg(plotData.extremeValue)
  );
  objectiveValueText->setPen(QPen(Qt::black)); //Show black border around text
  objectiveValueText->setSelectable(false);
  customPlot->addItem(objectiveValueText);

  //Add arrows pointing to the objective function extreme points:
  for (
    auto it(plotData.extremePoints.cbegin());
    it != plotData.extremePoints.cend();
    ++it
  )
  {
    QCPItemLine* const objectiveValueArrow = new QCPItemLine(customPlot);
    objectiveValueArrow->start->setParentAnchor(objectiveValueText->bottomLeft);
    objectiveValueArrow->end->setCoords((*it).x(), (*it).y());
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
GUI::MainWindow::setupSimplexView()
{
  _simplexTableModels = QVector<SimpleTableModel*>(SimplexModelsCount);
  for (int i(0); i < SimplexModelsCount; ++i)
  {
    _simplexTableModels[i] = new SimpleTableModel(this);
  }

  _simplexTableModels[int(SimplexModel::Solution)]->insertRow(0);
  _simplexTableModels[int(SimplexModel::Solution)]->insertColumn(0);
  _simplexTableModels[int(SimplexModel::Solution)]->setEditable(false);
  _simplexTableModels[int(SimplexModel::Solution)]->setSelectable(false);
  ui->simplex_solutionVectorTableView->setModel(
    _simplexTableModels[int(SimplexModel::Solution)]
  );

  _simplexTableModels[int(SimplexModel::ObjectiveValue)]->insertRow(0);
  _simplexTableModels[int(SimplexModel::ObjectiveValue)]->insertColumn(0);
  _simplexTableModels[int(SimplexModel::ObjectiveValue)]->setEditable(false);
  _simplexTableModels[int(SimplexModel::ObjectiveValue)]->setSelectable(false);
  ui->simplex_objectiveValueTableView->setModel(
    _simplexTableModels[int(SimplexModel::ObjectiveValue)]
  );

  _simplexTableModels[int(SimplexModel::Tableau)]->insertRow(0);
  _simplexTableModels[int(SimplexModel::Tableau)]->insertColumn(0);
  _simplexTableModels[int(SimplexModel::Tableau)]->setEditable(false);
  ui->simplex_simplexTableauTableView->setModel(
    _simplexTableModels[int(SimplexModel::Tableau)]
  );
}


void
GUI::MainWindow::enableStepByStepSimplexView(bool enabled)
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
GUI::MainWindow::enableCurrentSolutionSimplexView(bool enabled)
{
  ui->simplex_solutionVectorTableView->setEnabled(enabled);
  ui->simplex_objectiveValueTableView->setEnabled(enabled);
}


void
GUI::MainWindow::updateSimplexSelectionRules()
{
  //TODO: ~
}


void
GUI::MainWindow::clearSimplexView()
{
  for (int i(0); i < SimplexModelsCount; ++i)
  {
    _simplexTableModels[i]->clear(QStringLiteral("0"));
  }
}


void
GUI::MainWindow::refreshSimplexView()
{
  switch (_field)
  {
    case Field::Real:
      if (
        !_realSolverController.isEmpty() &&
        _realSolverController.stateChanged()
      )
      {
        const SimplexTableau<Real> tableau(_realSolverController.current());

        _simplexTableModels[int(SimplexModel::Tableau)]->resize(
          tableau.rows(), tableau.cols()
        );

        TableModelUtils::fill<Real>(
          _simplexTableModels[int(SimplexModel::Tableau)],
          tableau
        );

        ui->simplex_simplexTableauTableView->resizeColumnsToContents();

        const Matrix<Real, Dynamic, 1> extremePoint(tableau.extremePoint());

        _simplexTableModels[int(SimplexModel::Solution)]->resize(
          1, extremePoint.rows()
        );

        TableModelUtils::fill<Real>(
          _simplexTableModels[int(SimplexModel::Solution)],
          extremePoint.transpose()
        );

        ui->simplex_solutionVectorTableView->resizeColumnsToContents();

        TableModelUtils::fill(
          _simplexTableModels[int(SimplexModel::ObjectiveValue)],
          numericCast<QString, Real>(
            tableau.extremeValue()
          )
        );

        DenseIndex idx(0);
        const DenseIndex rows(tableau.rows());
        const DenseIndex cols(tableau.cols());
        TableModelUtils::setFlags<Real>(
          _simplexTableModels[int(SimplexModel::Tableau)],
          tableau.entries(),
          [rows, cols, &idx, &tableau]
          (const Real& x) {
            const DenseIndex rowIdx(idx / cols);
            const DenseIndex colIdx(idx % cols);
            const bool ret(
              ((rowIdx < rows - 1) && (colIdx < cols - 1)) &&
                MathUtils::isLessThanZero<Real>(
                  tableau.row(tableau.rows() - 1)(colIdx)
                ) &&
                MathUtils::isGreaterThanZero<Real>(x)
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
        _realSolverController.hasPrevious()
      );

      ui->simplex_stepForwardPushButton->setEnabled(
        _realSolverController.hasNext()
      );

      ui->simplex_iterCountLcdNumber->display(
        int(_realSolverController.iterationsCount())
      );

      ui->simplex_manualPivotCheckBox->setEnabled(
        _realSolverController.hasNext()
      );

      ui->simplex_pivotHintPushButton->setEnabled(
        _realSolverController.hasNext()
      );
      break;

    case Field::Rational:
      if (
        !_rationalSolverController.isEmpty() &&
        _rationalSolverController.stateChanged()
      )
      {
        const SimplexTableau<Rational> tableau(
          _rationalSolverController.current()
        );

        _simplexTableModels[int(SimplexModel::Tableau)]->resize(
          tableau.rows(), tableau.cols()
        );

        TableModelUtils::fill<Rational>(
          _simplexTableModels[int(SimplexModel::Tableau)],
          tableau
        );

        ui->simplex_simplexTableauTableView->resizeColumnsToContents();

        const Matrix<Rational, Dynamic, 1> extremePoint(tableau.extremePoint());

        _simplexTableModels[int(SimplexModel::Solution)]->resize(
          1, extremePoint.rows()
        );

        TableModelUtils::fill<Rational>(
          _simplexTableModels[int(SimplexModel::Solution)],
          extremePoint.transpose()
        );

        ui->simplex_solutionVectorTableView->resizeColumnsToContents();

        TableModelUtils::fill(
          _simplexTableModels[int(SimplexModel::ObjectiveValue)],
          numericCast<QString, Rational>(
            tableau.extremeValue()
          )
        );

        DenseIndex idx(0);
        const DenseIndex rows(tableau.rows());
        const DenseIndex cols(tableau.cols());
        TableModelUtils::setFlags<Rational>(
          _simplexTableModels[int(SimplexModel::Tableau)],
          tableau.entries(),
          [rows, cols, &idx, &tableau]
          (const Rational& x) {
            const DenseIndex rowIdx(idx / cols);
            const DenseIndex colIdx(idx % cols);
            const bool ret(
              ((rowIdx < rows - 1) && (colIdx < cols - 1)) &&
                MathUtils::isLessThanZero<Rational>(
                  tableau.row(tableau.rows() - 1)(colIdx)
                ) &&
                MathUtils::isGreaterThanZero<Rational>(x)
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
        _rationalSolverController.hasPrevious()
      );

      ui->simplex_stepForwardPushButton->setEnabled(
        _rationalSolverController.hasNext()
      );

      ui->simplex_iterCountLcdNumber->display(
        int(_rationalSolverController.iterationsCount())
      );

      ui->simplex_manualPivotCheckBox->setEnabled(
        _rationalSolverController.hasNext()
      );

      ui->simplex_pivotHintPushButton->setEnabled(
        _rationalSolverController.hasNext()
      );
      break;

    default:
      break;
  }
}


void
GUI::MainWindow::destroySimplexView()
{
  for (int i(0); i < SimplexModelsCount; ++i)
  {
    delete _simplexTableModels[i];
  }
}


void
GUI::MainWindow::assignTableModelsHeaders()
{
  _programTableModels[int(ProgramModel::ObjFunc)]->setHeaderData(
    0, Qt::Vertical, QStringLiteral("c")
  );
  _programTableModels[int(ProgramModel::RHS)]->setHeaderData(
    0, Qt::Horizontal, QStringLiteral("β")
  );

  _simplexTableModels[int(SimplexModel::Solution)]->setHeaderData(
    0, Qt::Vertical, QStringLiteral("x*")
  );
  _simplexTableModels[int(SimplexModel::ObjectiveValue)]->setHeaderData(
    0, Qt::Vertical, QStringLiteral("F*")
  );
}


void
GUI::MainWindow::convertTableModelsContents()
{
  switch (_field)
  {
    case Field::Real:
      for (int i(0); i < ProgramModelsCount; ++i)
      {
        TableModelUtils::convert<Real, Rational>(_programTableModels[i]);
      }
      break;

    case Field::Rational:
      for (int i(0); i < ProgramModelsCount; ++i)
      {
        TableModelUtils::convert<Rational, Real>(_programTableModels[i]);
      }
      break;

    default:
      break;
  }
}


void
GUI::MainWindow::toggleTableViewsDelegates()
{
  switch (_field)
  {
    case Field::Real:
      qInfo() << "MainWindow::toggleTableViewsDelegates:"
                 " switching to `Real'";
      ui->program_objFuncCoeffsTableView->setItemDelegate(
        _realNumericDelegates[int(ProgramModel::ObjFunc)]
      );
      ui->program_constrsCoeffsTableView->setItemDelegate(
        _realNumericDelegates[int(ProgramModel::Constrs)]
      );
      ui->program_constrsRHSTableView->setItemDelegate(
        _realNumericDelegates[int(ProgramModel::RHS)]
      );
      break;

    case Field::Rational:
      qInfo() << "MainWindow::toggleTableViewsDelegates:"
                  " switching to `Rational'";
      ui->program_objFuncCoeffsTableView->setItemDelegate(
        _ratNumericDelegates[int(ProgramModel::ObjFunc)]
      );
      ui->program_constrsCoeffsTableView->setItemDelegate(
        _ratNumericDelegates[int(ProgramModel::Constrs)]
      );
      ui->program_constrsRHSTableView->setItemDelegate(
        _ratNumericDelegates[int(ProgramModel::RHS)]
      );
      break;

    default:
      qCritical() << "MainWindow::toggleTableViewsDelegates:"
                     " unknown value of `_currentField'";
      break;
  }
}


void
GUI::MainWindow::toggleField()
{
  convertTableModelsContents();
  toggleTableViewsDelegates();

  clearSimplexView();
  enableCurrentSolutionSimplexView(false);
  enableStepByStepSimplexView(false);
}


void
GUI::MainWindow::setField(Field field)
{
  if (_field != field)
  {
    _field = field;

    toggleField();
  }
}


void
GUI::MainWindow::setDirty(bool dirty)
{
  if (_isDirty != dirty)
  {
    _isDirty = dirty;

    setWindowModified(dirty);
  }
}


void
GUI::MainWindow::setupNumericSolvers()
{
  _realSolver = make_shared<DantzigNumericSolver<Real>>();
  _rationalSolver = make_shared<DantzigNumericSolver<Rational>>();
}


void
GUI::MainWindow::setupNumericSolversControllers()
{
  _realSolverController = DantzigNumericSolverController<Real>(_realSolver);
  _rationalSolverController =
      DantzigNumericSolverController<Rational>(_rationalSolver);
}


void GUI::MainWindow::updateNumericSolversData()
{
  switch (_field)
  {
    case Field::Real:
      {
        Matrix<Real, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<Real>(
            _programTableModels[int(ProgramModel::ObjFunc)]
          )
        );
        Matrix<Real, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<Real>(
            _programTableModels[int(ProgramModel::Constrs)]
          )
        );
        Matrix<Real, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<Real>(
            _programTableModels[int(ProgramModel::RHS)]
          )
        );

        LinearProgramData<Real> linearProgramData(
          std::move(objFuncCoeffs),
          std::move(constrsCoeffs),
          std::move(constrsRHS)
        );
        _realSolver->setLinearProgramData(std::move(linearProgramData));

        break;
      }

    case Field::Rational:
      {
        Matrix<Rational, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<Rational>(
            _programTableModels[int(ProgramModel::ObjFunc)]
          )
        );
        Matrix<Rational, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<Rational>(
            _programTableModels[int(ProgramModel::Constrs)]
          )
        );
        Matrix<Rational, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<Rational>(
            _programTableModels[int(ProgramModel::RHS)]
          )
        );

        LinearProgramData<Rational> linearProgramData(
          std::move(objFuncCoeffs),
          std::move(constrsCoeffs),
          std::move(constrsRHS)
        );
        _rationalSolver->setLinearProgramData(linearProgramData);

        break;
      }

    default:
      break;
  }
}


void
GUI::MainWindow::solveSimplexHandler()
{
  updateNumericSolversData();

  ui->detailsTabWidget->setCurrentIndex(int(DetailsView::Simplex));
  ui->simplexMethodTab->setEnabled(true);

  enableCurrentSolutionSimplexView();
  ui->simplex_startPushButton->setEnabled(true);

  switch (_field)
  {
    case Field::Real:
      {
        const pair<SolutionType, optional<LinearProgramSolution<Real>>>
        linearProgramSolution(_realSolver->solve());

        if (linearProgramSolution.second)
        {
          _simplexTableModels[int(SimplexModel::Solution)]->resize(
            1,
            (*linearProgramSolution.second).extremePoint.rows()
          );
          TableModelUtils::fill<Real>(
            _simplexTableModels[int(SimplexModel::Solution)],
            (*linearProgramSolution.second).extremePoint.transpose()
          );

          TableModelUtils::fill(
            _simplexTableModels[int(SimplexModel::ObjectiveValue)],
            numericCast<QString, Real>(
              (*linearProgramSolution.second).extremeValue
            )
          );

          LOG(
            "Solution: x* := {0}\nF* := {1}",
            (*linearProgramSolution.second).extremePoint,
            (*linearProgramSolution.second).extremeValue
          );
        }
        else
        {
          solutionErrorHandler(
            QStringLiteral("This linear program can not be solved by"
                           " the Simplex method."),
            linearProgramSolution.first
          );
          goto error;
        }
        break;
      }

    case Field::Rational:
      {
        const pair<SolutionType, optional<LinearProgramSolution<Rational>>>
        linearProgramSolution(_rationalSolver->solve());

        if (linearProgramSolution.second)
        {
          _simplexTableModels[int(SimplexModel::Solution)]->resize(
            1,
            (*linearProgramSolution.second).extremePoint.rows()
          );
          TableModelUtils::fill<Rational>(
            _simplexTableModels[int(SimplexModel::Solution)],
            (*linearProgramSolution.second).extremePoint.transpose()
          );

          TableModelUtils::fill(
            _simplexTableModels[int(SimplexModel::ObjectiveValue)],
            numericCast<QString, Rational>(
              (*linearProgramSolution.second).extremeValue
            )
          );

          LOG(
            "Solution: x* := {0}\nF* := {1}",
            (*linearProgramSolution.second).extremePoint,
            (*linearProgramSolution.second).extremeValue
          );
        }
        else
        {
          solutionErrorHandler(
            QStringLiteral("This linear program can not be solved by"
                           " the Simplex method."),
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
GUI::MainWindow::solveGraphicalHandler()
{
  ui->detailsTabWidget->setCurrentIndex(int(DetailsView::Graphical));
  ui->graphicalMethodTab->setEnabled(true);

  enableGraphicalSolutionView();

  switch (_field)
  {
    case Field::Real:
      {
        Matrix<Real, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<Real>(
            _programTableModels[int(ProgramModel::ObjFunc)]
          )
        );

        Matrix<Real, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<Real>(
            _programTableModels[int(ProgramModel::Constrs)]
          )
        );

        Matrix<Real, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<Real>(
            _programTableModels[int(ProgramModel::RHS)]
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

        pair<SolutionType, optional<PlotDataReal2D>> plotData2D(
          graphicalSolver2D.solve()
        );

        if (plotData2D.second)
        {
          refreshGraphicalSolutionView(*plotData2D.second);
        }
        else
        {
          solutionErrorHandler(
            QStringLiteral("This linear program can not be solved by"
                           " the 2D graphical method."),
            plotData2D.first
          );

          goto error;
        }

        break;
      }

    case Field::Rational:
      {
        Matrix<Rational, 1, Dynamic> objFuncCoeffs(
          TableModelUtils::makeRowVector<Rational>(
            _programTableModels[int(ProgramModel::ObjFunc)]
          )
        );

        Matrix<Rational, Dynamic, Dynamic> constrsCoeffs(
          TableModelUtils::makeMatrix<Rational>(
            _programTableModels[int(ProgramModel::Constrs)]
          )
        );

        Matrix<Rational, Dynamic, 1> constrsRHS(
          TableModelUtils::makeColumnVector<Rational>(
            _programTableModels[int(ProgramModel::RHS)]
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

        pair<SolutionType, optional<PlotDataReal2D>> plotData2D(
          graphicalSolver2D.solve()
        );

        if (plotData2D.second)
        {
          refreshGraphicalSolutionView(*plotData2D.second);
        }
        else
        {
          solutionErrorHandler(
            QStringLiteral("This linear program can not be solved by"
                           " the 2D graphical method."),
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
GUI::MainWindow::solutionErrorHandler(
  const QString& description, LinearProgramming::SolutionType type
)
{
  QString details;
  switch (type) {
    case SolutionType::Unknown:
      details = QStringLiteral("Some error has encountered..."
                               "\nThe program could not be solved.");
      break;

    case SolutionType::Incomplete:
      details = QStringLiteral("Maximal iterations limit is exceeded.");
      break;

    case SolutionType::Infeasible:
      details = QStringLiteral("This linear program is infeasible:"
                               "\nThe feasible region `U' is empty set.");
      break;

    case SolutionType::Unbounded:
      details = QStringLiteral("The objective function `F' is unbounded"
                               " over the feasible region `U'.");
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
    description + QStringLiteral("\n") + details
  );
}


void
GUI::MainWindow::pivotingErrorHandler(
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
      details = QStringLiteral("The objective function `F' is unbounded"
                               " over the feasible region `U'.");
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
    description + QStringLiteral("\n") + details
  );
}


void
GUI::MainWindow::openFileHandler(const QString& filename)
{
  const ResultType result(loadData(filename));
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
    default:
      break;
  }
}


void
GUI::MainWindow::saveFileHandler(const QString& filename)
{
  const ResultType result(saveData(filename));
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
        QStringLiteral("Could not save file to the selected location.")
      );

    case ResultType::Nothing:
    default:
      break;
  }
}


Utils::ResultType
GUI::MainWindow::loadData(const QString& fileName)
{
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
      qWarning() << "MainWindow::loadData:"
                    " couldn't open file at path" << fileName;

      return ResultType::Nothing;
    }
    else
    {
      const QByteArray byteArray(file.readAll());
      if (byteArray.count() > 0)
      {
        const QJsonDocument jsonDocument(QJsonDocument::fromJson(byteArray));
        TableModelStorage tableModelStorage;
        const ResultType res(
          tableModelStorage.read(jsonDocument.object())
        );
        file.flush();
        file.close();

        if (res == ResultType::Success)
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
            assignTableModelsHeaders();

            ui->program_constrsSpinBox->setValue(
              _programTableModels[int(ProgramModel::Constrs)]->rowCount()
            );

            ui->program_varsSpinBox->setValue(
              _programTableModels[int(ProgramModel::Constrs)]->columnCount()
            );

            _field = tableModelStorage.field();
            switch (_field)
            {
              case Field::Real:
                ui->program_realRadioButton->setChecked(true);
                break;

              case Field::Rational:
                ui->program_rationalRadioButton->setChecked(true);
                break;

              default:
                qWarning() << "MainWindow::loadData: unknown value of `Field'";
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
GUI::MainWindow::saveData(const QString& fileName)
{
  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
      qWarning() << "MainWindow::saveData: couldn't save file"
                    " at path" << fileName;

      return ResultType::Nothing;
    }
    else
    {
      QJsonObject jsonObject;
      //TODO: ~? Use pointers here
      TableModelStorage tableModelStorage(
        QVector<SimpleTableModel>{
          (*_programTableModels[int(ProgramModel::ObjFunc)]),
          (*_programTableModels[int(ProgramModel::Constrs)]),
          (*_programTableModels[int(ProgramModel::RHS)])
        },
        _field
      );
      const ResultType res(tableModelStorage.write(jsonObject));

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
GUI::MainWindow::loadSettings()
{
  QSettings settings(
    QSettings::IniFormat, QSettings::UserScope, OrgName, AppName
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
GUI::MainWindow::saveSettings()
{
  QSettings settings(
    QSettings::IniFormat, QSettings::UserScope, OrgName, AppName
  );

  settings.beginGroup(QStringLiteral("mainWindow"));
  settings.setValue(QStringLiteral("geometry"), saveGeometry());
  settings.setValue(QStringLiteral("windowState"), saveState());
  settings.endGroup();
}

#pragma endregion


#pragma region public slots

void
GUI::MainWindow::on_graphical_solutionPlotQCustomPlot_selectionChangedByUser()
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
GUI::MainWindow::on_graphical_solutionPlotQCustomPlot_mousePress()
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
GUI::MainWindow::on_graphical_solutionPlotQCustomPlot_mouseWheel()
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
GUI::MainWindow::on_graphical_solutionPlotQCustomPlot_mouseMove(
  QMouseEvent* mouseEvent
)
{
  const double x(
    ui->graphical_solutionPlotQCustomPlot->xAxis->pixelToCoord(
      mouseEvent->pos().x()
    )
  );
  const double y(
    ui->graphical_solutionPlotQCustomPlot->yAxis->pixelToCoord(
      mouseEvent->pos().y()
    )
  );

  ui->graphical_solutionPlotQCustomPlot->setToolTip(
    QString("(%1; %2)").arg(x).arg(y)
  );
}


void
GUI::MainWindow::on_anyProgramModel_dataChanged(
  const QModelIndex& topLeft,
  const QModelIndex& bottomRight, const QVector<int>& roles)
{
  if (_isLoaded)
  {
    setDirty();

    ui->program_objFuncCoeffsTableView->resizeColumnsToContents();
    ui->program_constrsCoeffsTableView->resizeColumnsToContents();
    ui->program_constrsRHSTableView->resizeColumnsToContents();
  }
}


void
GUI::MainWindow::on_anyProgramModel_dimensionsChanged(
  const QModelIndex& parent, int start, int end
)
{
  if (_isLoaded)
  {
    setDirty();
  }
}


void
GUI::MainWindow::on_anyProgramModel_modelReset()
{
  if (_isLoaded)
  {
    setDirty();

    ui->program_objFuncCoeffsTableView->resizeColumnsToContents();
    ui->program_constrsCoeffsTableView->resizeColumnsToContents();
    ui->program_constrsRHSTableView->resizeColumnsToContents();
  }
}


void
GUI::MainWindow::on_program_constrsSpinBox_valueChanged(int arg1)
{
  if (arg1 >= MinConstraints && arg1 <= MaxConstraints)
  {
    if (ui->program_varsSpinBox->value() <= arg1) //If (N <= M')
    {
      ui->program_varsSpinBox->setValue(arg1); //N := M'
    }

    const int oldRowsCount(
      _programTableModels[int(ProgramModel::Constrs)]->rowCount()
    );
    const int oldColsCount(
      _programTableModels[int(ProgramModel::Constrs)]->columnCount()
    );

    _programTableModels[int(ProgramModel::Constrs)]->resize(arg1, 0);
    _programTableModels[int(ProgramModel::Constrs)]->clear(
      oldRowsCount, 0, arg1, oldColsCount, QStringLiteral("0")
    );

    _programTableModels[int(ProgramModel::RHS)]->resize(arg1, 0);
    _programTableModels[int(ProgramModel::RHS)]->clear(
      oldRowsCount, 0, arg1, 1, QStringLiteral("0")
    );
  }
}


void
GUI::MainWindow::on_program_varsSpinBox_valueChanged(int arg1)
{
  if (arg1 >= MinVariables && arg1 <= MaxVariables)
  {
    if (ui->program_constrsSpinBox->value() > arg1) //If (M > N')
    {
      ui->program_constrsSpinBox->setValue(arg1); //M := N'
    }

    const int oldColsCount(
      _programTableModels[int(ProgramModel::Constrs)]->columnCount()
    );
    const int oldRowsCount(
      _programTableModels[int(ProgramModel::Constrs)]->rowCount()
    );

    _programTableModels[int(ProgramModel::Constrs)]->resize(0, arg1);
    _programTableModels[int(ProgramModel::Constrs)]->clear(
      0, oldColsCount, oldRowsCount, arg1, QStringLiteral("0")
    );

    _programTableModels[int(ProgramModel::ObjFunc)]->resize(0, arg1);
    _programTableModels[int(ProgramModel::ObjFunc)]->clear(
      0, oldColsCount, 1, arg1, QStringLiteral("0")
    );
  }
}


void
GUI::MainWindow::on_program_realRadioButton_toggled(bool checked)
{
  if (checked)
  {
    setField(Field::Real);
  }
}


void
GUI::MainWindow::on_program_rationalRadioButton_toggled(bool checked)
{
  if (checked)
  {
    setField(Field::Rational);
  }
}


void
GUI::MainWindow::on_control_solveSimplexPushButton_clicked()
{
  solveSimplexHandler();
}


void
GUI::MainWindow::on_control_solveGraphicalPushButton_clicked()
{
  solveGraphicalHandler();
}


void
GUI::MainWindow::on_control_clearPushButton_clicked()
{
  clearProgramView();
}


[[deprecated("To be removed in release version!")]]
void
GUI::MainWindow::on_control_testPushButton_clicked()
{
  {
//    _loadData(QStringLiteral("data4.json"));
//    loadData(QStringLiteral("data3+.json"));
//    loadData(QStringLiteral("data5.json"));
    loadData(QStringLiteral("data6.json"));

    setDirty(false);
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
//    LOG("M=\n{0}\nA=\n{1}\nrk(A)=\n{2}\n",
//      m.format(MathematicaFormat),
//      rref.first.format(MathematicaFormat),
//      rref.second
//    );
  }
}


void
GUI::MainWindow::on_simplex_startPushButton_clicked()
{
  updateNumericSolversData();

  enableCurrentSolutionSimplexView();
  enableStepByStepSimplexView();

  switch (_field)
  {
    case Field::Real:
      _realSolverController.reset();
      _realSolverController.start();
      refreshSimplexView();
      break;

    case Field::Rational:
      _rationalSolverController.reset();
      _rationalSolverController.start();
      refreshSimplexView();
      break;

    default:
      break;
  }
}


void
GUI::MainWindow::on_simplex_stepBackPushButton_clicked()
{
  switch (_field)
  {
    case Field::Real:
      if (_realSolverController.hasPrevious())
      {
        _realSolverController.previous();
      }
      refreshSimplexView();
      break;

    case Field::Rational:
      if (_rationalSolverController.hasPrevious())
      {
        _rationalSolverController.previous();
      }
      refreshSimplexView();
      break;

    default:
      break;
  }
}


void
GUI::MainWindow::on_simplex_stepForwardPushButton_clicked()
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

  switch (_field)
  {
    case Field::Real:
      if (_realSolverController.hasNext())
      {
        _realSolverController.next(pivotIdx);
      }
      refreshSimplexView();
      break;

    case Field::Rational:
      if (_rationalSolverController.hasNext())
      {
        _rationalSolverController.next(pivotIdx);
      }
      refreshSimplexView();
      break;

    default:
      break;
  }
}


void
GUI::MainWindow::on_simplex_manualPivotCheckBox_toggled(bool checked)
{
  if (!checked)
  {
    ui->simplex_simplexTableauTableView->selectionModel()->clearSelection();
    ui->simplex_simplexTableauTableView->selectionModel()->clearCurrentIndex();
  }
}


void
GUI::MainWindow::on_simplex_pivotHintPushButton_clicked()
{
  switch (_field)
  {
    case Field::Real:
      if (!_realSolverController.isEmpty())
      {
        const pair<SolutionType, MaybeIndex2D> pivot(
          _realSolverController.pivot()
        );

        if (pivot.second)
        {
          //TODO: ~ Depending on the specified command, the index can also
          //become part of the current selection.
          const QModelIndex idx(
            _simplexTableModels[int(SimplexModel::Tableau)]->index(
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
      if (!_rationalSolverController.isEmpty())
      {
        const pair<SolutionType, MaybeIndex2D> pivot(
          _rationalSolverController.pivot()
        );

        if (pivot.second)
        {
          //TODO: ~ Depending on the specified command, the index can also
          //become part of the current selection.
          const QModelIndex idx(
            _simplexTableModels[int(SimplexModel::Tableau)]->index(
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
GUI::MainWindow::on_action_Open_triggered()
{
  //TODO: ~ Check for unsaved changes
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

  openFileHandler(filename);
}


void
GUI::MainWindow::on_action_Save_as_triggered()
{
  const QString filename(
    QFileDialog::getSaveFileName(
      this,
      QStringLiteral("Save Linear Program As..."),
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
        QString("/linearProgram@%1.json").arg(
          QDateTime::currentDateTime().toString(QStringLiteral("dMMMyy_h-m-s"))
        ),
      QStringLiteral("JSON file (*.json);;Plain text document (*.txt)"),
      0,
      QFileDialog::DontUseNativeDialog
    )
  );

  saveFileHandler(filename);
}


void
GUI::MainWindow::on_action_Quit_triggered()
{
  QApplication::quit();
}


void
GUI::MainWindow::on_action_Undo_triggered()
{
  //TODO: ~ Undo action
}


void
GUI::MainWindow::on_action_Redo_triggered()
{
  //TODO: ~ Redo action
}


void
GUI::MainWindow::on_action_Clear_triggered()
{
  clearProgramView();
}


void
GUI::MainWindow::on_action_Fill_w_random_numbers_triggered()
{
  for (int i(0); i < ProgramModelsCount; ++i)
  {
    TableModelUtils::fill(
      _programTableModels[i],
      TableModelUtils::FillMethod::Random
    );
  }
}


void
GUI::MainWindow::on_action_Zoom_in_triggered()
{
  //TODO: ~ Connect to QCP
}


void
GUI::MainWindow::on_action_Zoom_out_triggered()
{
  //TODO: ~ Connect to QCP
}


void
GUI::MainWindow::on_action_Zoom_reset_triggered()
{
  //TODO: ~ Connect to QCP
}


void
GUI::MainWindow::on_action_Solve_triggered()
{
  solveSimplexHandler();
}


void
GUI::MainWindow::on_action_Plot_triggered()
{
  solveGraphicalHandler();
}


void
GUI::MainWindow::on_action_How_to_triggered()
{
  //TODO: ~ Add how-to guide
}


void
GUI::MainWindow::on_action_About_triggered()
{
  QMessageBox::about(
    this,
    QStringLiteral("About Application..."),
    QStringLiteral(
      "<h3><b>Linear Optimization v. 0.0.1.</b></h3>"
      "<br><br>"
      "This application demonstrates some methods of linear programs solving."
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
      "<li><a href=\"http://louisdx.github.io/cxx-prettyprint\">"
      "cxx-prettyprint (v. master-334db03)</a></li>"
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
GUI::MainWindow::on_action_About_Qt_triggered()
{
  QApplication::aboutQt();
}

#pragma endregion
