#include "Tools.h"
#include "LayerManager.h"
#include "Layer.h"
#include "Commands.h"
#include "ColorManager.h"
#include <QDebug>
#include <QStack>
#include <QPoint>
#include <qapplication.h>
#include <qpainter.h>

// -------------------
// EyedropperTool
// -------------------
EyedropperTool::EyedropperTool(LayerManager* layers, ColorManager* colors, QObject* parent)
    : Tool(parent)
    , m_layerManager(layers)
    , m_colorManager(colors)
{
}

void EyedropperTool::mousePress(const QPoint& pos)
{
    if (!m_layerManager || !m_colorManager) return;

    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    QImage img = layer->image();
    if (pos.x() < 0 || pos.y() < 0 || pos.x() >= img.width() || pos.y() >= img.height())
        return;

    QColor pickedColor = img.pixelColor(pos);
    m_colorManager->setPrimaryColor(pickedColor);
}

// -------------------
// PencilTool
// -------------------
PencilTool::PencilTool(LayerManager* layers, CommandManager* commands, ColorManager* colors, ToolManager* toolManager, QObject* parent)
    : Tool(parent)
    , m_layerManager(layers)
    , m_commandManager(commands)
    , m_colorManager(colors)
    , m_toolManager(toolManager)
{
}


void PencilTool::mousePress(const QPoint& pos)
{
    if (!m_layerManager || !m_commandManager || !m_colorManager) return;

    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    m_drawing = true;
    m_lastPos = pos;
    m_startImage = layer->image();
}

void PencilTool::mouseMove(const QPoint& pos)
{
    if (!m_drawing || !m_layerManager || !m_colorManager || !m_toolManager) return;

    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    QPainter painter(&layer->image());
    int brushSize = m_toolManager->brushSize(); // берём размер кисти из ToolManager
    painter.setPen(QPen(m_colorManager->primaryColor(), brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(m_lastPos, pos);
    m_lastPos = pos;

    m_layerManager->layersChanged();
}


void PencilTool::mouseRelease(const QPoint& pos)
{
    if (!m_drawing || !m_layerManager || !m_commandManager || !m_colorManager) return;

    m_drawing = false;
    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    QImage newImage = layer->image();
    auto* cmd = new DrawCommand(m_layerManager, activeIndex, m_startImage, newImage);
    m_commandManager->ExecuteCommand(cmd);
}

// -------------------
// FillTool
// -------------------
FillTool::FillTool(LayerManager* layers, CommandManager* commands, ColorManager* colors, ToolManager* tools, QObject* parent)
    : Tool(parent)
    , m_layerManager(layers)
    , m_commandManager(commands)
    , m_colorManager(colors)
    , m_toolManager(tools)
{
}

void FillTool::mousePress(const QPoint& pos)
{
    if (!m_layerManager || !m_commandManager || !m_colorManager) return;

    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    m_startImage = layer->image();
    floodFill(layer->image(), pos, m_colorManager->primaryColor());
    m_layerManager->layersChanged();
}

void FillTool::mouseRelease(const QPoint& pos)
{
    Q_UNUSED(pos)
    if (!m_layerManager || !m_commandManager) return;

    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    auto* cmd = new DrawCommand(m_layerManager, activeIndex, m_startImage, layer->image());
    m_commandManager->ExecuteCommand(cmd);
}

void FillTool::floodFill(QImage& image, const QPoint& start, const QColor& color)
{
    if (start.x() < 0 || start.y() < 0 || start.x() >= image.width() || start.y() >= image.height())
        return;

    int tolerance = m_toolManager->tolerance();
    QColor targetColor = image.pixelColor(start);

    auto withinTolerance = [&](const QColor& a, const QColor& b) {
        return (std::abs(a.red()   - b.red())   <= tolerance &&
                std::abs(a.green() - b.green()) <= tolerance &&
                std::abs(a.blue()  - b.blue())  <= tolerance);
    };

    if (withinTolerance(targetColor, color))
        return;
    QStack<QPoint> stack;
    stack.push(start);

    while (!stack.isEmpty()) {
        QPoint p = stack.pop();
        if (p.x() < 0 || p.y() < 0 || p.x() >= image.width() || p.y() >= image.height())
            continue;
        if (!withinTolerance(image.pixelColor(p), targetColor))
            continue;

        image.setPixelColor(p, color);

        stack.push(QPoint(p.x() + 1, p.y()));
        stack.push(QPoint(p.x() - 1, p.y()));
        stack.push(QPoint(p.x(), p.y() + 1));
        stack.push(QPoint(p.x(), p.y() - 1));
    }
}

// -------------------
// BrushTool (мягкая кисть)
// -------------------
BrushTool::BrushTool(LayerManager* layers, CommandManager* commands, ColorManager* colors, ToolManager* toolManager, QObject* parent)
    : Tool(parent)
    , m_layerManager(layers)
    , m_commandManager(commands)
    , m_colorManager(colors)
    , m_toolManager(toolManager)
{
}

void BrushTool::mousePress(const QPoint& pos)
{
    if (!m_layerManager || !m_commandManager || !m_colorManager) return;

    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    m_drawing = true;
    m_lastPos = pos;
    m_startImage = layer->image();
}

void BrushTool::mouseMove(const QPoint& pos)
{
    if (!m_drawing || !m_layerManager || !m_colorManager || !m_toolManager) return;

    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    int brushSize = m_toolManager->brushSize();
    QColor color = m_colorManager->primaryColor();

    QPainter painter(&layer->image());
    painter.setRenderHint(QPainter::Antialiasing, true);

    const int steps = qMax(1, (pos - m_lastPos).manhattanLength() / 4);
    for (int i = 0; i <= steps; ++i) {
        qreal t = i / qreal(steps);
        QPointF point = m_lastPos * (1 - t) + pos * t;

        QRadialGradient gradient(point, brushSize / 2.0);

        const qreal k = 2.0;

        const int stepsGradient = 10;
        for (int j = 0; j <= stepsGradient; ++j) {
            qreal g = j / qreal(stepsGradient);
            qreal alpha = qPow(1.0 - g, k) * 255;
            QColor stepColor = color;
            stepColor.setAlpha(int(alpha));
            gradient.setColorAt(g, stepColor);
        }

        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(point, brushSize / 2.0, brushSize / 2.0);
    }


    m_lastPos = pos;
    m_layerManager->layersChanged();
}

void BrushTool::mouseRelease(const QPoint& pos)
{
    if (!m_drawing || !m_layerManager || !m_commandManager || !m_colorManager) return;

    m_drawing = false;
    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    QImage newImage = layer->image();
    auto* cmd = new DrawCommand(m_layerManager, activeIndex, m_startImage, newImage);
    m_commandManager->ExecuteCommand(cmd);
}


// -------------------
// EraserTool
// -------------------
EraserTool::EraserTool(LayerManager* layers, CommandManager* commands, ToolManager* toolManager, QObject* parent)
    : Tool(parent)
    , m_layerManager(layers)
    , m_commandManager(commands)
    , m_toolManager(toolManager)
{
}

void EraserTool::mousePress(const QPoint& pos)
{
    if (!m_layerManager || !m_commandManager) return;

    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    m_erasing = true;
    m_lastPos = pos;
    m_startImage = layer->image();
}

void EraserTool::mouseMove(const QPoint& pos)
{
    if (!m_erasing || !m_layerManager || !m_toolManager) return;

    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    QPainter painter(&layer->image());
    int brushSize = m_toolManager->brushSize();
    painter.setCompositionMode(QPainter::CompositionMode_Clear); // стираем пиксели
    painter.setPen(QPen(Qt::transparent, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(m_lastPos, pos);
    m_lastPos = pos;

    m_layerManager->layersChanged();
}

void EraserTool::mouseRelease(const QPoint& pos)
{
    if (!m_erasing || !m_layerManager || !m_commandManager) return;

    m_erasing = false;
    int activeIndex = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(activeIndex);
    if (!layer) return;

    QImage newImage = layer->image();
    auto* cmd = new DrawCommand(m_layerManager, activeIndex, m_startImage, newImage);
    m_commandManager->ExecuteCommand(cmd);
}

// -------------------
// LineTool
// -------------------
LineTool::LineTool(LayerManager* lm, CommandManager* cm, ColorManager* col, ToolManager* tm, QObject* parent)
    : Tool(parent), m_layerManager(lm), m_commandManager(cm), m_colorManager(col), m_toolManager(tm)
{
}

void LineTool::mousePress(const QPoint& pos)
{
    if (!m_layerManager) return;
    int idx = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(idx);
    if (!layer) return;

    m_startPos = pos;
    m_lastPos = pos;
    m_startImage = layer->image().copy(); // обязательно copy()
    m_drawing = true;
}

void LineTool::mouseMove(const QPoint& pos)
{
    if (!m_drawing || !m_layerManager) return;
    int idx = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(idx);
    if (!layer) return;

    m_lastPos = pos;

    QImage tmp = m_startImage;
    {
        QPainter p(&tmp);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(m_colorManager->primaryColor(), m_toolManager->brushSize(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(m_startPos, m_lastPos);
    }

    layer->setImage(tmp);
    m_layerManager->layersChanged();
}

void LineTool::mouseRelease(const QPoint& pos)
{
    if (!m_drawing || !m_layerManager || !m_commandManager) return;
    int idx = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(idx);
    if (!layer) return;

    m_lastPos = pos;
    m_drawing = false;

    QImage tmp = m_startImage;
    {
        QPainter p(&tmp);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(m_colorManager->primaryColor(), m_toolManager->brushSize(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(m_startPos, m_lastPos);
    }

    layer->setImage(tmp);
    m_layerManager->layersChanged();

    m_commandManager->ExecuteCommand(new DrawCommand(m_layerManager, idx, m_startImage, tmp));
}

// -------------------
// RectTool
// -------------------
static QRect normalizedSquare(const QPoint& a, const QPoint& b, bool shift)
{
    QRect r = QRect(a, b).normalized();
    if (shift) {
        int s = qMin(r.width(), r.height());
        if (r.width() > r.height()) {
            if (b.x() < a.x()) r.setLeft(r.right() - s);
            else r.setRight(r.left() + s);
        } else {
            if (b.y() < a.y()) r.setTop(r.bottom() - s);
            else r.setBottom(r.top() + s);
        }
    }
    return r;
}

RectTool::RectTool(LayerManager* lm, CommandManager* cm, ColorManager* col, ToolManager* tm, QObject* parent)
    : Tool(parent), m_layerManager(lm), m_commandManager(cm), m_colorManager(col), m_toolManager(tm)
{
}



void RectTool::mousePress(const QPoint& pos)
{
    if (!m_layerManager) return;
    int idx = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(idx);
    if (!layer) return;

    m_startPos = pos;
    m_lastPos = pos;
    m_startImage = layer->image().copy();
    m_drawing = true;
}

void RectTool::mouseMove(const QPoint& pos)
{
    if (!m_drawing || !m_layerManager) return;
    int idx = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(idx);
    if (!layer) return;

    m_lastPos = pos;

    QImage tmp = m_startImage;
    {
        QPainter p(&tmp);
        p.setRenderHint(QPainter::Antialiasing, false);
        p.setPen(QPen(m_colorManager->secondaryColor(), m_toolManager->brushSize(), Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        p.setBrush(m_colorManager->primaryColor());
        QRect rect = normalizedSquare(m_startPos, m_lastPos, QApplication::keyboardModifiers() & Qt::ShiftModifier);
        p.drawRect(rect);
    }

    layer->setImage(tmp);
    m_layerManager->layersChanged();
}

void RectTool::mouseRelease(const QPoint& pos)
{
    if (!m_drawing || !m_layerManager || !m_commandManager) return;
    int idx = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(idx);
    if (!layer) return;

    m_lastPos = pos;
    m_drawing = false;

    QImage tmp = m_startImage;
    {
        QPainter p(&tmp);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(m_colorManager->secondaryColor(), m_toolManager->brushSize(), Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        p.setBrush(m_colorManager->primaryColor());
        QRect rect = normalizedSquare(m_startPos, m_lastPos, QApplication::keyboardModifiers() & Qt::ShiftModifier);
        p.drawRect(rect);
    }

    layer->setImage(tmp);
    m_layerManager->layersChanged();

    m_commandManager->ExecuteCommand(new DrawCommand(m_layerManager, idx, m_startImage, tmp));
}

// -------------------
// EllipseTool
// -------------------
EllipseTool::EllipseTool(LayerManager* lm, CommandManager* cm, ColorManager* col, ToolManager* tm, QObject* parent)
    : Tool(parent), m_layerManager(lm), m_commandManager(cm), m_colorManager(col), m_toolManager(tm)
{
}

void EllipseTool::mousePress(const QPoint& pos)
{
    if (!m_layerManager) return;
    int idx = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(idx);
    if (!layer) return;

    m_startPos = pos;
    m_lastPos = pos;
    m_startImage = layer->image().copy();
    m_drawing = true;
}

void EllipseTool::mouseMove(const QPoint& pos)
{
    if (!m_drawing || !m_layerManager) return;
    int idx = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(idx);
    if (!layer) return;

    m_lastPos = pos;

    QImage tmp = m_startImage;
    {
        QPainter p(&tmp);
        p.setRenderHint(QPainter::Antialiasing, false);
        p.setPen(QPen(m_colorManager->secondaryColor(), m_toolManager->brushSize(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(m_colorManager->primaryColor());
        QRect rect = normalizedSquare(m_startPos, m_lastPos, QApplication::keyboardModifiers() & Qt::ShiftModifier);
        p.drawEllipse(rect);
    }

    layer->setImage(tmp);
    m_layerManager->layersChanged();
}

void EllipseTool::mouseRelease(const QPoint& pos)
{
    if (!m_drawing || !m_layerManager || !m_commandManager) return;
    int idx = m_layerManager->activeLayerIndex();
    Layer* layer = m_layerManager->layerAt(idx);
    if (!layer) return;

    m_lastPos = pos;
    m_drawing = false;

    QImage tmp = m_startImage;
    {
        QPainter p(&tmp);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(m_colorManager->secondaryColor(), m_toolManager->brushSize(), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(m_colorManager->primaryColor());
        QRect rect = normalizedSquare(m_startPos, m_lastPos, QApplication::keyboardModifiers() & Qt::ShiftModifier);
        p.drawEllipse(rect);
    }

    layer->setImage(tmp);
    m_layerManager->layersChanged();

    m_commandManager->ExecuteCommand(new DrawCommand(m_layerManager, idx, m_startImage, tmp));
}
