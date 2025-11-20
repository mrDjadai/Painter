#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QShortcut>
#include <qpainter.h>
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    layerManager = new LayerManager(this);
    commandManager = new CommandManager();
    setWindowTitle("Painter");
    showMaximized();

    SetShortcuts();
    SetupLayerUI();

    InitializeLayers();
}

void MainWindow::onLayersChanged()
{
    if (layerView)
    {
        layerView->update();
    }
}

void MainWindow::InitializeLayers()
{
    QSize canvasSize(800, 600);

    Layer* backgroundLayer = layerManager->createBackgroundLayer(canvasSize, Qt::green);
    if (backgroundLayer) {
        backgroundLayer->setName("Background");
        backgroundLayer->setOpacity(1.0f);
    }

    Layer* backgroundLayer1 = layerManager->createBackgroundLayer(canvasSize, Qt::red);
    if (backgroundLayer1) {
        backgroundLayer1->setName("Background1");
        backgroundLayer1->setOpacity(1.0f);
    }
    layerManager->setActiveLayer(0);
}

void MainWindow::SetupLayerDockWidget()
{
    layerWidget = new LayerWidget(layerManager, this);

    QDockWidget* layersDock = new QDockWidget(tr("Layers"), this);
    layersDock->setWidget(layerWidget);
    layersDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    addDockWidget(Qt::RightDockWidgetArea, layersDock);

    layersDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
}

void MainWindow::SetupLayerUI()
{
    layerView = new LayerView(layerManager, this);

    QDockWidget* layersDock = new QDockWidget(tr("Layers"), this);
    layersDock->setWidget(layerView);
    layersDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    addDockWidget(Qt::LeftDockWidgetArea, layersDock);

    layersDock->setFeatures(QDockWidget::NoDockWidgetFeatures);

    int initialWidth = width() * 0.75;
    layersDock->setFixedWidth(initialWidth);

    SetupLayerDockWidget();

    connect(layerManager, &LayerManager::layersChanged,
            this, &MainWindow::onLayersChanged);
}

void  MainWindow::SetShortcuts()
{
    QShortcut *ctrlZ = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(ctrlZ, &QShortcut::activated, this, &MainWindow::HandleUndo);

    QShortcut *ctrlY = new QShortcut(QKeySequence("Ctrl+Y"), this);
    connect(ctrlY, &QShortcut::activated, this, &MainWindow::HandleRedo);
}

void MainWindow::HandleUndo()
{
    commandManager->Undo();
}

void MainWindow::HandleRedo()
{
    commandManager->Redo();
}

MainWindow::~MainWindow()
{
    delete ui;
}
