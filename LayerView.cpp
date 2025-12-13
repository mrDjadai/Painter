#include "LayerView.h"
#include <QPainter>
#include "Config.h"

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
    m_fillTool = new FillTool(m_layerManager, m_commandManager, m_colorManager, m_toolManager, this);
    m_eyedropperTool = new EyedropperTool(m_layerManager, m_colorManager, this);

    m_brushtool = new BrushTool(m_layerManager, m_commandManager, m_colorManager, m_toolManager, this);
    m_erasertool = new EraserTool(m_layerManager, m_commandManager, m_toolManager, this);

    m_linetool = new LineTool(m_layerManager, m_commandManager, m_colorManager, m_toolManager, this);
    m_recttool = new RectTool(m_layerManager, m_commandManager, m_colorManager, m_toolManager, this);
    m_ellipsetool = new EllipseTool(m_layerManager, m_commandManager, m_colorManager, m_toolManager, this);
    updateCurrentTool();

    if (m_toolManager) {
        connect(m_toolManager, &ToolManager::toolChanged, this, &LayerView::updateCurrentTool);
    }
}

void LayerView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Определяем размеры холста по первому слою
    if (!m_layerManager || m_layerManager->layerCount() == 0) {
        painter.fillRect(rect(), Qt::white);
        return;
    }

    const Layer* layer0 = m_layerManager->layerAt(0);
    QSize canvasSize = layer0->image().size();

    // Вычисляем масштаб для сохранения пропорций
    float scaleX = float(width()) / canvasSize.width();
    float scaleY = float(height()) / canvasSize.height();
    float scale = qMin(scaleX, scaleY);

    // Размер изображения на виджете
    int imgWidth = int(canvasSize.width() * scale);
    int imgHeight = int(canvasSize.height() * scale);

    // Координаты для центрирования
    int offsetX = (width() - imgWidth) / 2;
    int offsetY = (height() - imgHeight) / 2;

    const int checkerSize = 10;
    QBrush checkerBrush(CHECK_COLOR_1, Qt::SolidPattern);
    QBrush checkerBrush2(CHECK_COLOR_2, Qt::SolidPattern);
    for (int y = 0; y < height(); y += checkerSize) {
        for (int x = 0; x < width(); x += checkerSize) {
            QRect rect(x, y, checkerSize, checkerSize);
            if (((x/10 + y/10) % 2) == 0)
                painter.fillRect(rect, checkerBrush);
            else
                painter.fillRect(rect, checkerBrush2);
        }
    }

    painter.save();
    painter.translate(offsetX, offsetY);
    painter.scale(scale, scale);

    for (int i = 0; i < m_layerManager->layerCount(); ++i) {
        const Layer* layer = m_layerManager->layerAt(i);
        if (!layer || !layer->isVisible()) continue;
        painter.setOpacity(layer->opacity());
        painter.drawImage(0, 0, layer->image());
    }

    painter.restore();
}


QPoint LayerView::toLayerCoordinates(const QPoint& pos) const
{
    if (!m_layerManager || m_layerManager->activeLayerIndex() < 0)
        return pos;

    const Layer* layer = m_layerManager->layerAt(m_layerManager->activeLayerIndex());
    QSize canvasSize = layer->image().size();
    QSize widgetSize = size();

    float scaleX = float(widgetSize.width()) / canvasSize.width();
    float scaleY = float(widgetSize.height()) / canvasSize.height();
    float scale = qMin(scaleX, scaleY);

    int imgWidth = int(canvasSize.width() * scale);
    int imgHeight = int(canvasSize.height() * scale);

    int offsetX = (widgetSize.width() - imgWidth) / 2;
    int offsetY = (widgetSize.height() - imgHeight) / 2;

    int x = int((pos.x() - offsetX) / scale);
    int y = int((pos.y() - offsetY) / scale);

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
    case ToolType::Line:
        m_currentTool = m_linetool;
        break;
    case ToolType::Rectangle:
        m_currentTool = m_recttool;
        break;
    case ToolType::Ellipse:
        m_currentTool = m_ellipsetool;
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

QImage LayerView::getCombinedImage() const
{
    if (!m_layerManager)
        return QImage();

    int width = 0;
    int height = 0;
    if (m_layerManager->layerCount() > 0) {
        const Layer* layer0 = m_layerManager->layerAt(0);
        width = layer0->image().width();
        height = layer0->image().height();
    }

    if (width == 0 || height == 0)
        return QImage();

    QImage combined(width, height, QImage::Format_ARGB32);


    QPainter painter(&combined);
    painter.setRenderHint(QPainter::Antialiasing);

    for (int i = 0; i < m_layerManager->layerCount(); ++i) {
        const Layer* layer = m_layerManager->layerAt(i);
        if (!layer) continue;

        painter.setOpacity(layer->opacity());
        painter.drawImage(0, 0, layer->image());
    }

    painter.end();
    return combined;
}
