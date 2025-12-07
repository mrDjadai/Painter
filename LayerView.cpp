#include "LayerView.h"
#include <QPainter>
#include <QDebug>

LayerView::LayerView(LayerManager* layerManager,
                     ToolManager* toolManager,
                     CommandManager* commandManager,
                     ColorManager* colorManager,
                     QWidget* parent)
    : QWidget(parent)
    , m_layerManager(layerManager)
    , m_toolManager(toolManager)
    , m_commandManager(commandManager)
    , m_colorManager(colorManager)
{
    setMinimumSize(400, 300);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("background-color: #2d2d2d;");

    // Инициализация инструментов
    m_pencilTool = new PencilTool(m_layerManager, m_commandManager, m_colorManager, m_toolManager, this);
    m_fillTool = new FillTool(m_layerManager, m_commandManager, m_colorManager, this);
    m_eyedropperTool = new EyedropperTool(m_layerManager, m_colorManager, this);
    m_brushtool = new BrushTool(m_layerManager, m_commandManager, m_colorManager, m_toolManager, this);
    m_erasertool = new EraserTool(m_layerManager, m_commandManager, m_toolManager, this);

    updateCurrentTool();

    if (m_toolManager) {
        connect(m_toolManager, &ToolManager::toolChanged, this, &LayerView::updateCurrentTool);
    }
}

QSize LayerView::sizeHint() const
{
    return QSize(800, 600);
}

void LayerView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), Qt::white);

    if (m_layerManager) {
        QRect targetRect = rect(); // растягиваем под весь виджет
        m_layerManager->renderLayers(painter, targetRect);
    }
}

// Преобразуем координаты мыши в координаты слоя
QPoint LayerView::toLayerCoordinates(const QPoint& pos) const
{
    if (!m_layerManager || m_layerManager->activeLayerIndex() < 0)
        return pos;

    const Layer* layer = m_layerManager->layerAt(m_layerManager->activeLayerIndex());
    QSize canvasSize = layer->image().size();
    QSize widgetSize = size();

    float scaleX = float(canvasSize.width()) / widgetSize.width();
    float scaleY = float(canvasSize.height()) / widgetSize.height();

    int x = int(pos.x() * scaleX);
    int y = int(pos.y() * scaleY);

    // Ограничиваем координаты рамками слоя
    x = qBound(0, x, canvasSize.width() - 1);
    y = qBound(0, y, canvasSize.height() - 1);

    return QPoint(x, y);
}

void LayerView::updateCurrentTool()
{
    if (!m_toolManager) return;

    switch (m_toolManager->currentTool()) {
    case ToolType::Pencil:
        m_currentTool = m_pencilTool;
        break;
    case ToolType::Brush:
        m_currentTool = m_brushtool;
        break;
    case ToolType::Eraser:
        m_currentTool = m_erasertool;
        break;
    case ToolType::Fill:
        m_currentTool = m_fillTool;
        break;
    case ToolType::Eyedropper:
        m_currentTool = m_eyedropperTool;
        break;
    default:
        m_currentTool = nullptr;
        break;
    }
}

void LayerView::mousePressEvent(QMouseEvent* event)
{
    if (m_currentTool) {
        QPoint layerPos = toLayerCoordinates(event->pos());
        m_currentTool->mousePress(layerPos);
    }
}

void LayerView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_currentTool) {
        QPoint layerPos = toLayerCoordinates(event->pos());
        m_currentTool->mouseMove(layerPos);
    }
}

void LayerView::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_currentTool) {
        QPoint layerPos = toLayerCoordinates(event->pos());
        m_currentTool->mouseRelease(layerPos);
    }
}
