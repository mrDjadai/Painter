#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include <QShortcut>
#include <qpainter.h>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QSplitter>
#include <QLabel>
#include "ToolsWidget.h"
#include "ColorPickerWidget.h"
#include <QFileDialog>
#include <QInputDialog>
#include "Config.h"


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

    toolManager = new ToolManager(this);
    colorManager = new ColorManager(this);
    layerManager = new LayerManager(this);
    commandManager = new CommandManager();

    setWindowTitle("Painter");

    showMaximized();


    SetShortcuts();
    SetupNewLayout();

    QMenu* fileMenu = menuBar()->addMenu("Файл");

    QAction* newProjectAction = new QAction("Новый проект", this);
    fileMenu->addAction(newProjectAction);
    connect(newProjectAction, &QAction::triggered, this, &MainWindow::createNewCanvasDialog);

    QAction* openProjectAction = new QAction("Открыть проект", this);
    fileMenu->addAction(openProjectAction);
    connect(openProjectAction, &QAction::triggered, this, &MainWindow::loadProjectDialog);

    QAction* saveAsAction = new QAction("Сохранить как", this);
    fileMenu->addAction(saveAsAction);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);


    QAction* exportAction = new QAction("Экспорт...", this);
    fileMenu->addAction(exportAction);
    connect(exportAction, &QAction::triggered, this, &MainWindow::exportCanvas);
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
}


void MainWindow::createNewCanvas(int w, int h)
{
    if (!layerManager) return;

    QSize size(w, h);

    layerManager->ClearLayers();
    commandManager->Clear();

    layerManager->createBackgroundLayer(size, Qt::white);


    layerManager->setActiveLayer(0);

    layerView->update();
}

void MainWindow::loadProject(const QString& filename)
{
    if (!layerManager) return;

    if (!layerManager->loadProject(filename)) {
        return;
    }

    layerView->update();
}

void MainWindow::SetupNewLayout()
{
    QWidget* centralWidget = new QWidget();
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. ВЕРТИКАЛЬНЫЙ SPLITTER (верхняя панель + основная область)
    QSplitter* mainSplitter = new QSplitter(Qt::Vertical);
    mainSplitter->setChildrenCollapsible(SPLITTER_COLLAPSIBLE);
    mainSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);

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

    QWidget* topPanel = new QWidget();
    topPanel->setObjectName("TopPanel");
    topPanel->setMinimumHeight(100);
    topPanel->setStyleSheet(
        "QWidget#TopPanel {"
        "   background-color: " TOP_PANEL_BACKGROUND ";"
        "   border-bottom: 1px solid " TOP_PANEL_BORDER ";"
        "}"
        );

    QHBoxLayout* topPanelLayout = new QHBoxLayout(topPanel);
    topPanelLayout->setContentsMargins(10, 5, 10, 5);
    topPanelLayout->setSpacing(0);

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

    QLabel* toolsTitle = new QLabel("Инструменты");
    toolsTitle->setStyleSheet(
        "color: " TOP_PANEL_TEXT_COLOR ";"
        "font-weight: bold;"
        "font-size: 12px;"
        "padding-bottom: 5px;"
        "border-bottom: 1px solid #555;"
        );
    toolsContainerLayout->addWidget(toolsTitle);

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
    verticalSeparator->setFixedWidth(1);

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

    QLabel* colorsTitle = new QLabel("Цвета");
    colorsTitle->setStyleSheet(
        "color: " TOP_PANEL_TEXT_COLOR ";"
        "font-weight: bold;"
        "font-size: 12px;"
        "padding-bottom: 5px;"
        "border-bottom: 1px solid #555;"
        );
    colorsContainerLayout->addWidget(colorsTitle);

    ColorPickerWidget* colorPickerWidget = new ColorPickerWidget(colorManager, colorsContainer);
    colorsContainerLayout->addWidget(colorPickerWidget);
    colorsContainerLayout->addStretch();

    topPanelLayout->addWidget(toolsContainer);


    topPanelLayout->addWidget(verticalSeparator);

    topPanelLayout->addWidget(colorsContainer);

    topPanelLayout->addStretch();

    // 1.2. ОСНОВНАЯ ОБЛАСТЬ (горизонтальный сплиттер)
    QSplitter* horizontalSplitter = new QSplitter(Qt::Horizontal);
    horizontalSplitter->setChildrenCollapsible(SPLITTER_COLLAPSIBLE);
    horizontalSplitter->setHandleWidth(SPLITTER_HANDLE_WIDTH);

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

    layerView = new LayerView(layerManager, toolManager, commandManager, colorManager, this);

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

    layerWidget = new LayerWidget(layerManager, commandManager, rightPanel);

    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(5, 5, 5, 5);
    rightLayout->setSpacing(5);

    QLabel* layersTitle = new QLabel("Слои");
    layersTitle->setStyleSheet(
        "font-weight: bold;"
        "font-size: 14px;"
        "color: " LAYERS_PANEL_TEXT_COLOR ";"
        "padding: 5px;"
        "background-color: #e0e0e0;"
        "border-bottom: 1px solid #ccc;"
        );
    rightLayout->addWidget(layersTitle);
    rightLayout->addWidget(layerWidget, 1);


    horizontalSplitter->addWidget(layerView);
    horizontalSplitter->addWidget(rightPanel);

    mainSplitter->addWidget(topPanel);
    mainSplitter->addWidget(horizontalSplitter);

    QList<int> horizontalSizes;
    horizontalSizes << width() * CANVAS_WIDTH_PERCENT / 100
                    << width() * LAYERS_PANEL_WIDTH_PERCENT / 100;
    horizontalSplitter->setSizes(horizontalSizes);

    QList<int> verticalSizes;
    verticalSizes << height() * 20 / 100
                  << height() * 80 / 100;
    mainSplitter->setSizes(verticalSizes);

    mainLayout->addWidget(mainSplitter, 1);

    connect(layerManager, &LayerManager::layersChanged,
            this, &MainWindow::onLayersChanged);
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
    if (!layerManager) return;

    QSize canvasSize(800, 600);

    Layer* backgroundLayer = layerManager->createBackgroundLayer(canvasSize, Qt::green);
    if (backgroundLayer) {
        backgroundLayer->setName("Background");
        backgroundLayer->setOpacity(1.0f);
    }

    layerManager->setActiveLayer(0);
}

void MainWindow::SetShortcuts()
{
    QShortcut *ctrlZ = new QShortcut(QKeySequence("Ctrl+Z"), this);
    connect(ctrlZ, &QShortcut::activated, this, &MainWindow::HandleUndo);

    QShortcut *ctrlY = new QShortcut(QKeySequence("Ctrl+Y"), this);
    connect(ctrlY, &QShortcut::activated, this, &MainWindow::HandleRedo);

    // Горячие клавиши для инструментов
    QShortcut *pencilShortcut = new QShortcut(QKeySequence("P"), this);
    connect(pencilShortcut, &QShortcut::activated, this, [this]() {
        if (toolManager) {
            toolManager->setCurrentTool(ToolType::Pencil);
        }
    });

    QShortcut *brushShortcut = new QShortcut(QKeySequence("B"), this);
    connect(brushShortcut, &QShortcut::activated, this, [this]() {
        if (toolManager) {
            toolManager->setCurrentTool(ToolType::Brush);
        }
    });

    QShortcut *eraserShortcut = new QShortcut(QKeySequence("E"), this);
    connect(eraserShortcut, &QShortcut::activated, this, [this]() {
        if (toolManager) {
            toolManager->setCurrentTool(ToolType::Eraser);
        }
    });

    QShortcut *fillShortcut = new QShortcut(QKeySequence("G"), this);
    connect(fillShortcut, &QShortcut::activated, this, [this]() {
        if (toolManager) {
            toolManager->setCurrentTool(ToolType::Fill);
        }
    });

    QShortcut *eyedropperShortcut = new QShortcut(QKeySequence("I"), this);
    connect(eyedropperShortcut, &QShortcut::activated, this, [this]() {
        if (toolManager) {
            toolManager->setCurrentTool(ToolType::Eyedropper);
        }
    });

    // Горячие клавиши для цветов
    QShortcut *swapColorsShortcut = new QShortcut(QKeySequence("X"), this);
    connect(swapColorsShortcut, &QShortcut::activated, this, [this]() {
        if (colorManager) {
            colorManager->swapColors();
        }
    });

    // Слои
    QShortcut *newLayerShortcut = new QShortcut(QKeySequence("Ctrl+Shift+N"), this);
    connect(newLayerShortcut, &QShortcut::activated, this, [this]() {
        if (layerManager && layerManager->layerCount() > 0) {
            QSize size = layerManager->layerAt(0)->image().size();
            Layer* newLayer = layerManager->createNewLayer(size, "New Layer");
        }
    });
}

void MainWindow::HandleUndo()
{
    if (commandManager->Undo())
    {
        if (layerView)
            layerView->update();
    }
}

void MainWindow::HandleRedo()
{
    if (commandManager->Redo())
    {
        if (layerView)
            layerView->update();
    }
}

MainWindow::~MainWindow()
{
    delete commandManager;
    delete ui;
}

void MainWindow::exportCanvas()
{
    if (!layerView) return;

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Экспорт холста",
        QString(),
        "PNG (*.png);;JPEG (*.jpg *.jpeg)"
        );

    if (fileName.isEmpty())
        return;

    // Определяем формат по расширению файла
    QString format;
    if (fileName.endsWith(".png", Qt::CaseInsensitive))
        format = "PNG";
    else if (fileName.endsWith(".jpg", Qt::CaseInsensitive) || fileName.endsWith(".jpeg", Qt::CaseInsensitive))
        format = "JPEG";
    else {
        fileName += ".png";
        format = "PNG";
    }

    QImage image = layerView->getCombinedImage();

    image.save(fileName, format.toUtf8().constData());
}

void MainWindow::createNewCanvasDialog()
{
    bool ok;
    int w = QInputDialog::getInt(this, "Новый проект", "Ширина:", 1280, 1, 16000, 1, &ok);
    if (!ok) return;
    int h = QInputDialog::getInt(this, "Новый проект", "Высота:", 720, 1, 16000, 1, &ok);
    if (!ok) return;

    createNewCanvas(w, h);
}

void MainWindow::loadProjectDialog()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Открыть проект",
        QString(),
        "Painter Project (*.ptr)"
        );
    if (!filename.isEmpty()) {
        commandManager->Clear();
        loadProject(filename);
    }
}

void MainWindow::openImageAsNewProject(const QString& filename)
{
    QImage img(filename);
    if (img.isNull())
        return;

    createNewCanvas(img.width(), img.height());


    LayerManager* lm = layerManager;
    if (!lm) return;

    int activeIndex = lm->activeLayerIndex();
    Layer* layer = lm->layerAt(activeIndex);

    if (!layer) return;

    layer->setImage(img);

    lm->layersChanged();
}

