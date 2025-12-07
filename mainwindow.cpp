#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QShortcut>
#include <qpainter.h>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include <QStatusBar>
#include <QDebug>
#include "toolswidget.h"
#include "colorpickerwidget.h"
#include <QFileDialog>

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
    , layerManager(nullptr)
    , commandManager(nullptr)
    , toolManager(nullptr)
    , colorManager(nullptr)
    , layerView(nullptr)
    , layerWidget(nullptr)
{
    ui->setupUi(this);

    // Создаем менеджеры
    toolManager = new ToolManager(this);
    colorManager = new ColorManager(this);
    layerManager = new LayerManager(this);
    commandManager = new CommandManager();

    setWindowTitle("Painter");

    showMaximized();

    // Создаем статус бар
    statusBar()->showMessage("Ready");

    SetShortcuts();
    SetupNewLayout();

    QMenu* fileMenu = menuBar()->addMenu("Файл");

    QAction* saveAsAction = new QAction("Сохранить как", this);
    fileMenu->addAction(saveAsAction);

    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);

    qDebug() << "Application initialized with Tools and Color systems";
}

void MainWindow::saveAs()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Сохранить проект как",
        QString(),
        "Painter Project (*.ptr)"
        );

    if (filename.isEmpty())
        return;

    if (!filename.endsWith(".ptr"))
        filename += ".ptr";

    if (layerManager->saveProject(filename))
        statusBar()->showMessage("Сохранено: " + filename);
    else
        statusBar()->showMessage("Ошибка сохранения!");
}


void MainWindow::createNewCanvas(int w, int h)
{
    if (!layerManager) return;

    QSize size(w, h);

    // Удаляем старые слои
//    layerManager->clear();

    // Создаем фоновый слой
    Layer* background = layerManager->createBackgroundLayer(size, Qt::white);
    background->setName("Background");

    layerManager->setActiveLayer(0);

    layerWidget->InitRow();
    layerView->update();

    statusBar()->showMessage(
        QString("Created new canvas %1×%2").arg(w).arg(h)
        );
}

void MainWindow::loadProject(const QString& filename)
{
    if (!layerManager) return;

    if (!layerManager->loadProject(filename)) {
        statusBar()->showMessage("Failed to load project");
        return;
    }

    layerWidget->InitRow();
    layerView->update();

    statusBar()->showMessage("Project loaded: " + filename);
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

    // 1.1. ВЕРХНЯЯ ПАНЕЛЬ ИНСТРУМЕНТОВ И ЦВЕТОВ
    QWidget* topPanel = new QWidget();
    topPanel->setObjectName("TopPanel");
    topPanel->setMinimumHeight(100); // Увеличили высоту для лучшего отображения
    topPanel->setStyleSheet(
        "QWidget#TopPanel {"
        "   background-color: " TOP_PANEL_BACKGROUND ";"
        "   border-bottom: 1px solid " TOP_PANEL_BORDER ";"
        "}"
        );

    // Горизонтальный layout для всей верхней панели
    QHBoxLayout* topPanelLayout = new QHBoxLayout(topPanel);
    topPanelLayout->setContentsMargins(10, 5, 10, 5);
    topPanelLayout->setSpacing(0); // Убираем spacing, будем контролировать через разделитель

    // ===== ЛЕВАЯ ЧАСТЬ: ИНСТРУМЕНТЫ =====
    QWidget* toolsContainer = new QWidget();
    toolsContainer->setObjectName("ToolsContainer");
    toolsContainer->setStyleSheet(
        "QWidget#ToolsContainer {"
        "   background-color: #333333;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        );

    QVBoxLayout* toolsContainerLayout = new QVBoxLayout(toolsContainer);
    toolsContainerLayout->setContentsMargins(5, 5, 5, 5);

    // Заголовок инструментов
    QLabel* toolsTitle = new QLabel("Tools");
    toolsTitle->setStyleSheet(
        "color: " TOP_PANEL_TEXT_COLOR ";"
        "font-weight: bold;"
        "font-size: 12px;"
        "padding-bottom: 5px;"
        "border-bottom: 1px solid #555;"
        );
    toolsContainerLayout->addWidget(toolsTitle);

    // Виджет инструментов
    ToolsWidget* toolsWidget = new ToolsWidget(toolManager, colorManager, toolsContainer);
    toolsContainerLayout->addWidget(toolsWidget);
    toolsContainerLayout->addStretch(); // Прижимаем к верху

    // ===== РАЗДЕЛИТЕЛЬ МЕЖДУ ИНСТРУМЕНТАМИ И ЦВЕТАМИ =====
    QFrame* verticalSeparator = new QFrame();
    verticalSeparator->setObjectName("VerticalSeparator");
    verticalSeparator->setFrameShape(QFrame::VLine);
    verticalSeparator->setFrameShadow(QFrame::Sunken);
    verticalSeparator->setStyleSheet(
        "QFrame#VerticalSeparator {"
        "   background-color: #555555;"
        "   border-left: 1px solid #777777;"
        "   border-right: 1px solid #444444;"
        "   margin: 10px 15px;"
        "}"
        );
    verticalSeparator->setFixedWidth(1); // Тонкий разделитель

    // ===== ПРАВАЯ ЧАСТЬ: ЦВЕТА =====
    QWidget* colorsContainer = new QWidget();
    colorsContainer->setObjectName("ColorsContainer");
    colorsContainer->setStyleSheet(
        "QWidget#ColorsContainer {"
        "   background-color: #333333;"
        "   border-radius: 5px;"
        "   padding: 5px;"
        "}"
        );

    QVBoxLayout* colorsContainerLayout = new QVBoxLayout(colorsContainer);
    colorsContainerLayout->setContentsMargins(5, 5, 5, 5);

    // Заголовок цветов
    QLabel* colorsTitle = new QLabel("Colors");
    colorsTitle->setStyleSheet(
        "color: " TOP_PANEL_TEXT_COLOR ";"
        "font-weight: bold;"
        "font-size: 12px;"
        "padding-bottom: 5px;"
        "border-bottom: 1px solid #555;"
        );
    colorsContainerLayout->addWidget(colorsTitle);

    // Виджет выбора цветов
    ColorPickerWidget* colorPickerWidget = new ColorPickerWidget(colorManager, colorsContainer);
    colorsContainerLayout->addWidget(colorPickerWidget);
    colorsContainerLayout->addStretch(); // Прижимаем к верху

    // Добавляем контейнер инструментов
    topPanelLayout->addWidget(toolsContainer);

    // Добавляем вертикальный разделитель
    topPanelLayout->addWidget(verticalSeparator);

    // Добавляем контейнер цветов
    topPanelLayout->addWidget(colorsContainer);

    // Добавляем растягивающееся пространство справа
    topPanelLayout->addStretch();

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
    layerView = new LayerView(layerManager, toolManager, commandManager, colorManager, this);

    // Передаем менеджеры инструментов и цветов в LayerView
    // (нужно будет добавить соответствующие методы в LayerView)
    // layerView->setToolManager(toolManager);
    // layerView->setColorManager(colorManager);

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
    rightLayout->addWidget(layerWidget, 1); // Растягиваем LayerWidget

    // Добавляем в горизонтальный сплиттер
    horizontalSplitter->addWidget(layerView);
    horizontalSplitter->addWidget(rightPanel);

    // Добавляем в главный вертикальный сплиттер
    mainSplitter->addWidget(topPanel);          // Верхняя панель (инструменты + цвета)
    mainSplitter->addWidget(horizontalSplitter); // Основная область (холст + слои)

    // Устанавливаем начальные пропорции
    QList<int> horizontalSizes;
    horizontalSizes << width() * CANVAS_WIDTH_PERCENT / 100
                    << width() * LAYERS_PANEL_WIDTH_PERCENT / 100;
    horizontalSplitter->setSizes(horizontalSizes);

    QList<int> verticalSizes;
    verticalSizes << height() * 20 / 100  // 20% для верхней панели (увеличили для лучшего вида)
                  << height() * 80 / 100; // 80% для основной области
    mainSplitter->setSizes(verticalSizes);

    // 2. Собираем всё вместе
    mainLayout->addWidget(mainSplitter, 1);   // Вертикальный сплиттер (растягивается на всё)

    // Подключаем сигналы
    connect(layerManager, &LayerManager::layersChanged,
            this, &MainWindow::onLayersChanged);

    // Подключаем сигналы менеджеров инструментов и цветов
    if (toolManager) {
        connect(toolManager, &ToolManager::toolChanged,
                this, &MainWindow::onToolChanged);
        connect(toolManager, &ToolManager::brushSizeChanged,
                this, &MainWindow::onBrushSizeChanged);
    }

    if (colorManager) {
        connect(colorManager, &ColorManager::primaryColorChanged,
                this, &MainWindow::onPrimaryColorChanged);
        connect(colorManager, &ColorManager::secondaryColorChanged,
                this, &MainWindow::onSecondaryColorChanged);
    }

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
        // Обновляем статус бар
        if (layerManager) {
            int layerCount = layerManager->layerCount();
            QString activeLayerName = layerManager->activeLayer() ?
                                          layerManager->activeLayer()->name() : "None";
            statusBar()->showMessage(QString("Layers: %1 | Active: %2")
                                         .arg(layerCount).arg(activeLayerName));
        }
    }
}

void MainWindow::onToolChanged(ToolType tool)
{
    if (toolManager) {
        QString toolName = toolManager->toolName(tool);
        statusBar()->showMessage(QString("Selected tool: %1").arg(toolName));
        qDebug() << "Tool changed to:" << toolName;
    }
}

void MainWindow::onBrushSizeChanged(int size)
{
    statusBar()->showMessage(QString("Brush size: %1").arg(size));
}

void MainWindow::onPrimaryColorChanged(const QColor& color)
{
    statusBar()->showMessage(QString("Primary color: %1").arg(color.name()));
}

void MainWindow::onSecondaryColorChanged(const QColor& color)
{
    statusBar()->showMessage(QString("Secondary color: %1").arg(color.name()));
}

void MainWindow::InitializeLayers()
{
    if (!layerManager) return;

    QSize canvasSize(800, 600);

    Layer* backgroundLayer = layerManager->createBackgroundLayer(canvasSize, Qt::green);
    if (backgroundLayer) {
        backgroundLayer->setName("Background");
        backgroundLayer->setOpacity(1.0f);
    }

    layerManager->setActiveLayer(0);
    layerWidget->InitRow();
    statusBar()->showMessage(QString("Created %1 initial layers").arg(layerManager->layerCount()));
}

void MainWindow::SetShortcuts()
{
    // Отмена/повтор
    QShortcut *ctrlZ = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(ctrlZ, &QShortcut::activated, this, &MainWindow::HandleUndo);

    QShortcut *ctrlY = new QShortcut(QKeySequence("Ctrl+Y"), this);
    connect(ctrlY, &QShortcut::activated, this, &MainWindow::HandleRedo);

    // Горячие клавиши для инструментов
    QShortcut *pencilShortcut = new QShortcut(QKeySequence("P"), this);
    connect(pencilShortcut, &QShortcut::activated, this, [this]() {
        if (toolManager) {
            toolManager->setCurrentTool(ToolType::Pencil);
            statusBar()->showMessage("Pencil tool selected (P)");
        }
    });

    QShortcut *brushShortcut = new QShortcut(QKeySequence("B"), this);
    connect(brushShortcut, &QShortcut::activated, this, [this]() {
        if (toolManager) {
            toolManager->setCurrentTool(ToolType::Brush);
            statusBar()->showMessage("Brush tool selected (B)");
        }
    });

    QShortcut *eraserShortcut = new QShortcut(QKeySequence("E"), this);
    connect(eraserShortcut, &QShortcut::activated, this, [this]() {
        if (toolManager) {
            toolManager->setCurrentTool(ToolType::Eraser);
            statusBar()->showMessage("Eraser tool selected (E)");
        }
    });

    QShortcut *fillShortcut = new QShortcut(QKeySequence("G"), this);
    connect(fillShortcut, &QShortcut::activated, this, [this]() {
        if (toolManager) {
            toolManager->setCurrentTool(ToolType::Fill);
            statusBar()->showMessage("Fill tool selected (G)");
        }
    });

    QShortcut *eyedropperShortcut = new QShortcut(QKeySequence("I"), this);
    connect(eyedropperShortcut, &QShortcut::activated, this, [this]() {
        if (toolManager) {
            toolManager->setCurrentTool(ToolType::Eyedropper);
            statusBar()->showMessage("Eyedropper tool selected (I)");
        }
    });

    // Горячие клавиши для цветов
    QShortcut *swapColorsShortcut = new QShortcut(QKeySequence("X"), this);
    connect(swapColorsShortcut, &QShortcut::activated, this, [this]() {
        if (colorManager) {
            colorManager->swapColors();
            statusBar()->showMessage("Colors swapped (X)");
        }
    });

    // Слои
    QShortcut *newLayerShortcut = new QShortcut(QKeySequence("Ctrl+Shift+N"), this);
    connect(newLayerShortcut, &QShortcut::activated, this, [this]() {
        if (layerManager && layerManager->layerCount() > 0) {
            QSize size = layerManager->layerAt(0)->image().size();
            Layer* newLayer = layerManager->createNewLayer(size, "New Layer");
            if (newLayer) {
                statusBar()->showMessage("New layer created (Ctrl+Shift+N)");
            }
        }
    });

    // Сохранение/загрузка цветов
    QShortcut *saveColorsShortcut = new QShortcut(QKeySequence("Ctrl+Shift+S"), this);
    connect(saveColorsShortcut, &QShortcut::activated, this, [this]() {
        if (colorManager) {
            colorManager->saveColors();
            statusBar()->showMessage("Colors saved (Ctrl+Shift+S)");
        }
    });

    QShortcut *loadColorsShortcut = new QShortcut(QKeySequence("Ctrl+Shift+L"), this);
    connect(loadColorsShortcut, &QShortcut::activated, this, [this]() {
        if (colorManager) {
            colorManager->loadColors();
            statusBar()->showMessage("Colors loaded (Ctrl+Shift+L)");
        }
    });

    // Обновление дисплея
    QShortcut *refreshShortcut = new QShortcut(QKeySequence("F5"), this);
    connect(refreshShortcut, &QShortcut::activated, this, [this]() {
        if (layerView) {
            layerView->update();
            statusBar()->showMessage("Display refreshed (F5)");
        }
    });
}

void MainWindow::HandleUndo()
{
    if (commandManager->Undo()) {
        statusBar()->showMessage("Undo performed (Ctrl+Z)");
        if (layerView) layerView->update();
    } else {
        statusBar()->showMessage("Nothing to undo");
    }
}

void MainWindow::HandleRedo()
{
    if (commandManager->Redo()) {
        statusBar()->showMessage("Redo performed (Ctrl+Y)");
        if (layerView) layerView->update();
    } else {
        statusBar()->showMessage("Nothing to redo");
    }
}

MainWindow::~MainWindow()
{
    // Сохраняем цвета перед выходом
    if (colorManager) {
        colorManager->saveColors();
        qDebug() << "Colors saved before exit";
    }

    delete commandManager;
    delete ui;
}
