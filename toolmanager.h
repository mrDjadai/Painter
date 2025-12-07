#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>
#include "ToolType.h"

class ToolManager : public QObject
{
    Q_OBJECT

public:
    explicit ToolManager(QObject* parent = nullptr);

    ToolType currentTool() const { return m_currentTool; }
    void setCurrentTool(ToolType tool);

    int brushSize() const { return m_brushSize; }
    void setBrushSize(int size);

    bool usePressure() const { return m_usePressure; }
    void setUsePressure(bool use);

    QString toolName(ToolType tool) const;

signals:
    void toolChanged(ToolType tool);
    void brushSizeChanged(int size);

private:
    ToolType m_currentTool;
    int m_brushSize;
    bool m_usePressure;
};

#endif // TOOLMANAGER_H
