#pragma once

#include <QWidget>
#include <QMouseEvent>
#include "LayerManager.h"
#include "ToolManager.h"
#include "CommandSystem.h"
#include "ColorManager.h"
#include "Tools.h"

class LayerView : public QWidget
{
    Q_OBJECT
public:
    explicit LayerView(LayerManager* layerManager,
                       ToolManager* toolManager,
                       CommandManager* commandManager,
                       ColorManager* colorManager,
                       QWidget* parent = nullptr);

    QSize sizeHint() const override;
    QImage getCombinedImage() const;
protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void updateCurrentTool();

private:
    // Преобразует координаты мыши виджета в координаты активного слоя
    QPoint toLayerCoordinates(const QPoint& pos) const;

    LayerManager* m_layerManager = nullptr;
    ToolManager* m_toolManager = nullptr;
    CommandManager* m_commandManager = nullptr;
    ColorManager* m_colorManager = nullptr;

    // Инструменты
    PencilTool* m_pencilTool = nullptr;
    FillTool* m_fillTool = nullptr;
    EyedropperTool* m_eyedropperTool = nullptr;

    BrushTool* m_brushtool = nullptr;
    EraserTool* m_erasertool = nullptr;

    LineTool* m_linetool = nullptr;
    RectTool* m_recttool = nullptr;
    EllipseTool* m_ellipsetool = nullptr;

    // Текущий инструмент
    Tool* m_currentTool = nullptr;
};
