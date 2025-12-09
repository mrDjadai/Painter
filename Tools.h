#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QPoint>
#include <QImage>
#include "toolmanager.h"
class LayerManager;
class CommandManager;
class ColorManager;

// Базовый интерфейс инструмента (по желанию)
class Tool : public QObject
{
    Q_OBJECT
public:
    explicit Tool(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~Tool() = default;

    virtual void mousePress(const QPoint& pos) = 0;
    virtual void mouseMove(const QPoint& pos) = 0;
    virtual void mouseRelease(const QPoint& pos) = 0;
};

// Пипетка — сразу меняет текущий цвет
class EyedropperTool : public Tool
{
    Q_OBJECT
public:
    EyedropperTool(LayerManager* layers, ColorManager* colors, QObject* parent = nullptr);

    void mousePress(const QPoint& pos) override;
    void mouseMove(const QPoint& pos) override {}
    void mouseRelease(const QPoint& pos) override {}

private:
    LayerManager* m_layerManager;
    ColorManager* m_colorManager;
};

// Карандаш — создаёт команду рисования для undo/redo
class PencilTool : public Tool
{
    Q_OBJECT
public:
    PencilTool(LayerManager* layers, CommandManager* commands, ColorManager* colors, ToolManager* toolManager, QObject* parent);

    void mousePress(const QPoint& pos) override;
    void mouseMove(const QPoint& pos) override;
    void mouseRelease(const QPoint& pos) override;

private:
    LayerManager* m_layerManager;
    CommandManager* m_commandManager;
    ColorManager* m_colorManager;
    ToolManager* m_toolManager;
    int m_brushSize;

    bool m_drawing = false;
    QPoint m_lastPos;
    QImage m_startImage;
};

class BrushTool : public Tool
{
    Q_OBJECT
public:
    BrushTool(LayerManager* layers, CommandManager* commands, ColorManager* colors, ToolManager* toolManager, QObject* parent);

    void mousePress(const QPoint& pos) override;
    void mouseMove(const QPoint& pos) override;
    void mouseRelease(const QPoint& pos) override;

private:
    LayerManager* m_layerManager;
    CommandManager* m_commandManager;
    ColorManager* m_colorManager;
    ToolManager* m_toolManager;
    int m_brushSize;

    bool m_drawing = false;
    QPoint m_lastPos;
    QImage m_startImage;
};

class EraserTool : public Tool
{
    Q_OBJECT
public:
    EraserTool(LayerManager* layers, CommandManager* commands, ToolManager* toolManager, QObject* parent);

    void mousePress(const QPoint& pos) override;
    void mouseMove(const QPoint& pos) override;
    void mouseRelease(const QPoint& pos) override;

private:
    LayerManager* m_layerManager;
    CommandManager* m_commandManager;
    ColorManager* m_colorManager;
    ToolManager* m_toolManager;
    int m_brushSize;

    bool m_erasing = false;
    QPoint m_lastPos;
    QImage m_startImage;
};

// Заливка — создаёт команду для undo/redo
class FillTool : public Tool
{
    Q_OBJECT
public:
    FillTool(LayerManager* layers, CommandManager* commands, ColorManager* colors, ToolManager* tools, QObject* parent = nullptr);

    void mousePress(const QPoint& pos) override;
    void mouseMove(const QPoint& pos) override {}
    void mouseRelease(const QPoint& pos) override;

private:
    LayerManager* m_layerManager;
    CommandManager* m_commandManager;
    ColorManager* m_colorManager;
    ToolManager* m_toolManager;

    QImage m_startImage;

    void floodFill(QImage& image, const QPoint& start, const QColor& color);
};

class LineTool : public Tool
{
    Q_OBJECT
public:
    LineTool(LayerManager* lm,
             CommandManager* cm,
             ColorManager* col,
             ToolManager* tm,
             QObject* parent = nullptr);

    void mousePress(const QPoint& pos) override;
    void mouseMove(const QPoint& pos) override;
    void mouseRelease(const QPoint& pos) override;

private:
    LayerManager* m_layerManager;
    CommandManager* m_commandManager;
    ColorManager* m_colorManager;
    ToolManager* m_toolManager;

    QPoint m_startPos;
    QPoint m_lastPos;

    QImage m_startImage;
    bool m_drawing = false;
};

class RectTool : public Tool
{
    Q_OBJECT
public:
    RectTool(LayerManager* lm,
             CommandManager* cm,
             ColorManager* col,
             ToolManager* tm,
             QObject* parent = nullptr);

    void mousePress(const QPoint& pos) override;
    void mouseMove(const QPoint& pos) override;
    void mouseRelease(const QPoint& pos) override;

private:
    LayerManager* m_layerManager;
    CommandManager* m_commandManager;
    ColorManager* m_colorManager;
    ToolManager* m_toolManager;

    QPoint m_startPos;
    QPoint m_lastPos;

    QImage m_startImage;
    bool m_drawing = false;
};

class EllipseTool : public Tool
{
    Q_OBJECT
public:
    EllipseTool(LayerManager* lm,
                CommandManager* cm,
                ColorManager* col,
                ToolManager* tm,
                QObject* parent = nullptr);

    void mousePress(const QPoint& pos) override;
    void mouseMove(const QPoint& pos) override;
    void mouseRelease(const QPoint& pos) override;

private:
    LayerManager* m_layerManager;
    CommandManager* m_commandManager;
    ColorManager* m_colorManager;
    ToolManager* m_toolManager;

    QPoint m_startPos;
    QPoint m_lastPos;

    QImage m_startImage;
    bool m_drawing = false;
};
#endif // TOOLS_H
