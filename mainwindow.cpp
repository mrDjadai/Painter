#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QShortcut>
#include <qpainter.h>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QSplitter>

// Параметры пропорций интерфейса
#define TOP_PANEL_HEIGHT_PERCENT 10    // Высота верхней панели (% от общей высоты)
#define CENTRAL_AREA_HEIGHT_PERCENT 90 // Высота центральной области (% от общей высоты)

#define CANVAS_WIDTH_PERCENT 85        // Ширина холста (% от ширины центральной области)
#define LAYERS_PANEL_WIDTH_PERCENT 15  // Ширина панели слоев (% от ширины центральной области)

#define SPLITTER_HANDLE_WIDTH 4        // Ширина разделителя
#define SPLITTER_COLLAPSIBLE false     // Можно ли сворачивать панели

// Минимальные размеры (в пикселях)
#define MIN_TOP_PANEL_HEIGHT 40
#define MIN_CANVAS_WIDTH 400
#define MIN_CANVAS_HEIGHT 300
#define MIN_LAYERS_PANEL_WIDTH 200

// Цвета интерфейса
#define TOP_PANEL_BACKGROUND "#2b2b2b"
#define TOP_PANEL_BORDER "#555"
#define TOP_PANEL_TEXT_COLOR "white"

#define LAYERS_PANEL_BACKGROUND "#f8f8f8"
#define LAYERS_PANEL_BORDER "#ccc"
#define LAYERS_PANEL_LEFT_BORDER "#ddd"
#define LAYERS_PANEL_TEXT_COLOR "#333"

#define CANVAS_BACKGROUND "white"
#define CANVAS_BORDER "#666"
#define SPLITTER_HANDLE_COLOR "#cccccc"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    layerManager = new LayerManager(this);
    commandManager = new CommandManager();
    setWindowTitle("Painter");

    // Устанавливаем начальный размер с учетом пропорций
    resize(1200, 800);

    SetShortcuts();
    SetupNewLayout();

    InitializeLayers();
}

void MainWindow::SetupNewLayout()
{
    // Создаем центральный виджет для новой системы layout
    QWidget* centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    // Главный вертикальный layout
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. ВЕРТИКАЛЬНЫЙ SPLITTER (верхняя панель + основная область)
    QSplitter* mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->setChildrenCollapsible(SPLITTER_COLLAPSIBLE);
    mainSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);

    // Стиль для вертикального разделителя
    mainSplitter->setStyleSheet(
        "QSplitter::handle {"
        "   background-color: #cccccc;"
        "   border-top: 2px solid #aaaaaa;"
        "   border-bottom: 2px solid #aaaaaa;"
        "}"
        "QSplitter::handle:hover {"
        "   background-color: #aaaaaa;"
        "}"
        "QSplitter::handle:vertical {"
        "   height: 8px;"
        "   margin: 0px;"
        "}"
        );

    // 1.1. ВЕРХНЯЯ ПАНЕЛЬ ИНСТРУМЕНТОВ (ИЗМЕНЯЕМАЯ ВЫСОТА)
    QWidget* topPanel = new QWidget();
    topPanel->setObjectName("TopPanel");
    topPanel->setMinimumHeight(40); // Минимальная высота
    topPanel->setStyleSheet(
        "QWidget#TopPanel {"
        "   background-color: " TOP_PANEL_BACKGROUND ";"
        "   border-bottom: 1px solid " TOP_PANEL_BORDER ";"
        "}"
        );

    QHBoxLayout* topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(5, 5, 5, 5);

    QLabel* titleLabel = new QLabel("Tools Panel - Drag the handle below to resize height");
    titleLabel->setStyleSheet(
        "color: " TOP_PANEL_TEXT_COLOR ";"
        "font-weight: bold;"
        );
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // 1.2. ОСНОВНАЯ ОБЛАСТЬ (горизонтальный сплиттер)
    QSplitter* horizontalSplitter = new QSplitter(Qt::Horizontal);
    horizontalSplitter->setChildrenCollapsible(SPLITTER_COLLAPSIBLE);
    horizontalSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);

    // Стиль для горизонтального разделителя
    horizontalSplitter->setStyleSheet(
        "QSplitter::handle {"
        "   background-color: #cccccc;"
        "   border-left: 2px solid #aaaaaa;"
        "   border-right: 2px solid #aaaaaa;"
        "}"
        "QSplitter::handle:hover {"
        "   background-color: #aaaaaa;"
        "}"
        "QSplitter::handle:horizontal {"
        "   width: 8px;"
        "   margin: 0px;"
        "}"
        );

    // ХОЛСТ (левая часть горизонтального сплиттера)
    layerView = new LayerView(layerManager, this);
    layerView->setMinimumSize(MIN_CANVAS_WIDTH, MIN_CANVAS_HEIGHT);
    layerView->setStyleSheet(
        "background-color: " CANVAS_BACKGROUND ";"
        "border: 1px solid " CANVAS_BORDER ";"
        );

    // ПАНЕЛЬ СЛОЕВ (правая часть горизонтального сплиттера)
    QWidget* rightPanel = new QWidget();
    rightPanel->setObjectName("RightPanel");
    rightPanel->setMinimumWidth(MIN_LAYERS_PANEL_WIDTH);
    rightPanel->setStyleSheet(
        "QWidget#RightPanel {"
        "   background-color: " LAYERS_PANEL_BACKGROUND ";"
        "   border: 1px solid " LAYERS_PANEL_BORDER ";"
        "}"
        );

    // Создаем LayerWidget внутри правой панели
    layerWidget = new LayerWidget(layerManager, commandManager, rightPanel);

    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(5, 5, 5, 5);
    rightLayout->setSpacing(5);

    QLabel* layersTitle = new QLabel("Layers");
    layersTitle->setStyleSheet(
        "font-weight: bold;"
        "font-size: 14px;"
        "color: " LAYERS_PANEL_TEXT_COLOR ";"
        "padding: 5px;"
        "background-color: #e0e0e0;"
        "border-bottom: 1px solid #ccc;"
        );
    rightLayout->addWidget(layersTitle);
    rightLayout->addWidget(layerWidget);

    // Добавляем в горизонтальный сплиттер
    horizontalSplitter->addWidget(layerView);
    horizontalSplitter->addWidget(rightPanel);

    // Добавляем в главный вертикальный сплиттер
    mainSplitter->addWidget(topPanel);          // Верхняя панель (изменяемая высота)
    mainSplitter->addWidget(horizontalSplitter); // Основная область

    // Устанавливаем начальные пропорции
    QList<int> horizontalSizes;
    horizontalSizes << width() * CANVAS_WIDTH_PERCENT / 100
                    << width() * LAYERS_PANEL_WIDTH_PERCENT / 100;
    horizontalSplitter->setSizes(horizontalSizes);

    QList<int> verticalSizes;
    verticalSizes << height() * TOP_PANEL_HEIGHT_PERCENT / 100  // 10% для верхней панели
                  << height() * CENTRAL_AREA_HEIGHT_PERCENT / 100; // 90% для основной области
    mainSplitter->setSizes(verticalSizes);

    // 2. Собираем всё вместе
    mainLayout->addWidget(mainSplitter, 1);   // Вертикальный сплиттер (растягивается на всё)

    // Подключаем сигналы
    connect(layerManager, &LayerManager::layersChanged,
            this, &MainWindow::onLayersChanged);

    // Отладочная информация
    connect(mainSplitter, &QSplitter::splitterMoved, this, [this, mainSplitter]() {
        QList<int> sizes = mainSplitter->sizes();
        int totalHeight = sizes[0] + sizes[1];
        int topPercent = (sizes[0] * 100) / totalHeight;
        int mainPercent = (sizes[1] * 100) / totalHeight;
        qDebug() << "Top panel:" << topPercent << "%, Main area:" << mainPercent << "%";
    });

    connect(horizontalSplitter, &QSplitter::splitterMoved, this, [this, horizontalSplitter]() {
        QList<int> sizes = horizontalSplitter->sizes();
        int totalWidth = sizes[0] + sizes[1];
        int canvasPercent = (sizes[0] * 100) / totalWidth;
        int layersPercent = (sizes[1] * 100) / totalWidth;
        qDebug() << "Canvas:" << canvasPercent << "%, Layers:" << layersPercent << "%";
    });
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
