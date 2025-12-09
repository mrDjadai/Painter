#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "LayerManager.h"
#include "CommandSystem.h"
#include "LayerView.h"
#include "LayerWidget.h"
#include "ToolManager.h"
#include "ColorManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void createNewCanvas(int w, int h);
    void loadProject(const QString& filename);
public slots:
    void createNewCanvasDialog();
    void loadProjectDialog();

private slots:
    void HandleUndo();
    void HandleRedo();
    void onLayersChanged();
    void onToolChanged(ToolType tool);
    void onBrushSizeChanged(int size);
    void onPrimaryColorChanged(const QColor& color);
    void onSecondaryColorChanged(const QColor& color);
    void saveAs();

private:
    void SetShortcuts();
    void SetupNewLayout();
    void InitializeLayers();
    void exportCanvas();

    Ui::MainWindow *ui;
    LayerManager* layerManager;
    CommandManager* commandManager;
    ToolManager* toolManager;
    ColorManager* colorManager;
    LayerView* layerView;
    LayerWidget* layerWidget;
};

#endif // MAINWINDOW_H
