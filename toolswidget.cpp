#include "ToolsWidget.h"
#include "ToolManager.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QToolButton>
#include <QButtonGroup>
#include <QSlider>
#include <QLabel>

ToolsWidget::ToolsWidget(ToolManager* toolManager, ColorManager* colorManager, QWidget* parent)
    : QWidget(parent)
    , m_toolManager(toolManager)
    , m_colorManager(colorManager)
{
    setupUI();
    setupConnections();
    updateToolSelection();

    if (m_toolManager) {
        connect(m_toolManager, &ToolManager::toolChanged,
                this, &ToolsWidget::updateToolSelection);
    }
}

void ToolsWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    // ----------------------------
    //    ГРУППА КНОПОК ИНСТРУМЕНТОВ
    // ----------------------------

    QButtonGroup* toolButtonGroup = new QButtonGroup(this);
    toolButtonGroup->setExclusive(true);

    QGridLayout* toolsGrid = new QGridLayout();
    toolsGrid->setSpacing(3);

    auto addTool = [&](ToolType type, const QString& text, int row, int col)
    {
        QToolButton* btn = new QToolButton();
        btn->setText(text);
        btn->setToolTip(text);
        btn->setCheckable(true);
        btn->setFixedSize(70, 28);
        btn->setProperty("toolType", static_cast<int>(type));

        toolButtonGroup->addButton(btn);
        toolsGrid->addWidget(btn, row, col);

        m_toolButtons[type] = btn;
    };

    // Добавляем инструменты — БЕЗ Selection и без Text
    addTool(ToolType::Pencil,     "Карандаш",     0, 0);
    addTool(ToolType::Brush,      "Кисть",      0, 1);
    addTool(ToolType::Eraser,     "Ластик",     0, 2);

    addTool(ToolType::Fill,       "Заливка",       1, 0);
    addTool(ToolType::Eyedropper, "Пипетка", 1, 1);
 //   addTool(ToolType::Line,       "Line",       1, 2);

  //  addTool(ToolType::Rectangle,  "Rectangle",  2, 0);
  //  addTool(ToolType::Ellipse,    "Ellipse",    2, 1);

    mainLayout->addLayout(toolsGrid);

    // ----------------------------
    //       СЛАЙДЕР РАЗМЕРА
    // ----------------------------

    m_brushSizeContainer = new QWidget();
    QHBoxLayout* brushSizeLayout = new QHBoxLayout(m_brushSizeContainer);
    brushSizeLayout->setContentsMargins(0,0,0,0);
    m_brushSizeContainer->setFixedHeight(10);

    m_sizeLabel = new QLabel("Размер:");
    m_sizeLabel->setStyleSheet("color: white;");
    brushSizeLayout->addWidget(m_sizeLabel);

    m_brushSizeSlider = new QSlider(Qt::Horizontal);
    m_brushSizeSlider->setRange(1, 50);
    m_brushSizeSlider->setValue(3);
    m_brushSizeSlider->setFixedWidth(110);
    brushSizeLayout->addWidget(m_brushSizeSlider);

    m_brushSizeLabel = new QLabel("3");
    m_brushSizeLabel->setStyleSheet("color: white;");
    m_brushSizeLabel->setFixedWidth(25);
    brushSizeLayout->addWidget(m_brushSizeLabel);

    brushSizeLayout->addStretch();

    mainLayout->addWidget(m_brushSizeContainer); // добавляем контейнер в layout
    mainLayout->addStretch();
}

void ToolsWidget::setupConnections()
{
    for (QToolButton* button : m_toolButtons.values()) {
        connect(button, &QToolButton::clicked, this, &ToolsWidget::onToolButtonClicked);
    }

    connect(m_brushSizeSlider, &QSlider::valueChanged, this, [this](int value) {
        if (m_toolManager) {
            m_toolManager->setBrushSize(value);
        }
        m_brushSizeLabel->setText(QString::number(value));
    });
}

bool ToolsWidget::toolHasBrushSize(ToolType tool)
{
    // Выбирай сам, какие инструменты используют размер
    return tool == ToolType::Pencil ||
           tool == ToolType::Brush ||
           tool == ToolType::Eraser ||
           tool == ToolType::Line;
}

void ToolsWidget::onToolButtonClicked()
{
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    if (!button || !m_toolManager) return;

    ToolType toolType = static_cast<ToolType>(button->property("toolType").toInt());
    m_toolManager->setCurrentTool(toolType);
}

void ToolsWidget::updateToolSelection()
{
    if (!m_toolManager) return;

    // Снимаем выделение
    for (QToolButton* button : m_toolButtons.values()) {
        button->setChecked(false);
    }

    // Выделяем активную кнопку
    ToolType currentTool = m_toolManager->currentTool();
    if (m_toolButtons.contains(currentTool)) {
        m_toolButtons[currentTool]->setChecked(true);
    }

    // Показываем/скрываем содержимое слайдера, но не контейнер
    bool show = toolHasBrushSize(currentTool);

    m_sizeLabel->setVisible(show);
    m_brushSizeSlider->setVisible(show);
    m_brushSizeLabel->setVisible(show);

    if (show) {
        m_brushSizeSlider->setValue(m_toolManager->brushSize());
    }
}
