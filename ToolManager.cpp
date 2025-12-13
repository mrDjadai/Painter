#include "toolmanager.h"

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

void ToolManager::setTolerance(int value)
{
    if (m_tolerance == value)
        return;

    m_tolerance = value;
}
