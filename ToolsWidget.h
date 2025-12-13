#ifndef TOOLSWIDGET_H
#define TOOLSWIDGET_H

#include <QWidget>
#include <QMap>
#include "ToolManager.h"
#include "ColorManager.h"

class QToolButton;
class QSlider;
class QLabel;

class ToolsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolsWidget(ToolManager* toolManager,
                         ColorManager* colorManager,
                         QWidget* parent = nullptr);

private slots:
    void onToolButtonClicked();
    void updateToolSelection();

private:
    void setupUI();
    void setupConnections();

    // Определяет, должен ли показываться слайдер размера для выбранного инструмента
    bool toolHasBrushSize(ToolType tool);

private:
    ToolManager* m_toolManager;
    ColorManager* m_colorManager;

    QMap<ToolType, QToolButton*> m_toolButtons;

    QWidget* m_brushSizeContainer = nullptr;
    QSlider* m_brushSizeSlider = nullptr;
    QLabel*  m_brushSizeLabel = nullptr;
    QLabel*  m_sizeLabel = nullptr;


    QWidget* m_fillToleranceContainer = nullptr;
    QSlider* m_fillToleranceSlider = nullptr;
    QLabel*  m_fillToleranceLabel = nullptr;
    QLabel*  m_fillToleranceValueLabel = nullptr;

};

#endif // TOOLSWIDGET_H
