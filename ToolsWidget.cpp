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
    addTool(ToolType::Line,       "Линия",       1, 2);

    addTool(ToolType::Rectangle,  "Прямоугольник",  2, 0);
    addTool(ToolType::Ellipse,    "Элипс",    2, 1);

    mainLayout->addLayout(toolsGrid);

    QWidget* slidersContainer = new QWidget();
    QHBoxLayout* slidersLayout = new QHBoxLayout(slidersContainer);
    slidersLayout->setContentsMargins(0, 0, 0, 0);
    slidersLayout->setSpacing(0); // отступ между блоками

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

    slidersLayout->addWidget(m_brushSizeContainer); // добавляем контейнер в layout

    // ----------------------------
    //   СЛАЙДЕР ДОПУСКА ЗАЛИВКИ
    // ----------------------------

    m_fillToleranceContainer = new QWidget();
    QHBoxLayout* tolLayout = new QHBoxLayout(m_fillToleranceContainer);
    tolLayout->setContentsMargins(0, 0, 0, 0);
    m_fillToleranceContainer->setFixedHeight(10);


    // Метка "Допуск:"
    m_fillToleranceLabel = new QLabel("Допуск:");
    m_fillToleranceLabel->setStyleSheet("color: white;");
    m_fillToleranceLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    tolLayout->addWidget(m_fillToleranceLabel);

    // Ползунок
    m_fillToleranceSlider = new QSlider(Qt::Horizontal);
    m_fillToleranceSlider->setRange(0, 255);
    m_fillToleranceSlider->setValue(0);
    m_fillToleranceSlider->setFixedWidth(110);
    m_fillToleranceSlider->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    tolLayout->addWidget(m_fillToleranceSlider);

    // Цифровое значение
    m_fillToleranceValueLabel = new QLabel(QString::number(m_fillToleranceSlider->value()));
    m_fillToleranceValueLabel->setStyleSheet("color: white;");
    m_fillToleranceValueLabel->setFixedWidth(25);
    m_fillToleranceValueLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    tolLayout->addWidget(m_fillToleranceValueLabel);

    // Растягиваем оставшееся пространство
    tolLayout->addStretch();

    slidersLayout->addWidget(m_fillToleranceContainer);

    mainLayout->addWidget(slidersContainer);
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

    connect(m_fillToleranceSlider, &QSlider::valueChanged, this,
            [this](int value){
                if (m_toolManager)
                    m_toolManager->setTolerance(value);
                m_fillToleranceValueLabel->setText(QString::number(value));
            }
            );

}

bool ToolsWidget::toolHasBrushSize(ToolType tool)
{
    switch (tool) {
    case ToolType::Pencil:
    case ToolType::Brush:
    case ToolType::Eraser:
    case ToolType::Line:
    case ToolType::Rectangle:
    case ToolType::Ellipse:
        return true;
    default:
        return false;
        break;
    }
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

    bool fillVisible = (currentTool == ToolType::Fill);
    m_fillToleranceContainer->setVisible(fillVisible);

    if (fillVisible) {
        m_fillToleranceSlider->setValue(m_toolManager->tolerance());
    }

    if (show) {
        m_brushSizeSlider->setValue(m_toolManager->brushSize());
    }
}
