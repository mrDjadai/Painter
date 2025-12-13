#ifndef TOOLMANAGER_H
#define TOOLMANAGER_H

#include <QObject>
#include "ToolType.h"

class ToolManager : public QObject
{
    Q_OBJECT

public:
    ToolManager(QObject* parent = nullptr);

    ToolType currentTool() const { return m_currentTool; }
    void setCurrentTool(ToolType tool);

    int brushSize() const { return m_brushSize; }
    void setBrushSize(int size);

    bool usePressure() const { return m_usePressure; }
    void setUsePressure(bool use);

    int tolerance() const{ return m_tolerance; }
    void setTolerance(int);


signals:
    void toolChanged(ToolType tool);
    void brushSizeChanged(int size);

private:
    ToolType m_currentTool;
    int m_brushSize;
    bool m_usePressure;
    int m_tolerance = 0;
};

#endif // TOOLMANAGER_H
