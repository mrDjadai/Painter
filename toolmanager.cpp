#include "toolmanager.h"
#include <QDebug>

ToolManager::ToolManager(QObject* parent)
    : QObject(parent)
    , m_currentTool(ToolType::Pencil)
    , m_brushSize(3)
    , m_usePressure(false)
{
}

void ToolManager::setCurrentTool(ToolType tool)
{
    if (m_currentTool != tool) {
        m_currentTool = tool;
        emit toolChanged(tool);
        qDebug() << "Tool changed to:" << toolName(tool);
    }
}

void ToolManager::setBrushSize(int size)
{
    size = qMax(1, qMin(size, 100));
    if (m_brushSize != size) {
        m_brushSize = size;
        emit brushSizeChanged(size);
    }
}

QString ToolManager::toolName(ToolType tool) const
{
    switch (tool) {
    case ToolType::Pencil: return "Pencil";
    case ToolType::Eraser: return "Eraser";
    case ToolType::Fill: return "Fill";
    case ToolType::Eyedropper: return "Eyedropper";
    case ToolType::Brush: return "Brush";
    case ToolType::Line: return "Line";
    case ToolType::Rectangle: return "Rectangle";
    case ToolType::Ellipse: return "Ellipse";
    default: return "Unknown";
    }
}
