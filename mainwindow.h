#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CommandSystem.h"
#include "LayerManager.h"
#include "LayerView.h"
#include "LayerWidget.h"

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

private:
    Ui::MainWindow *ui;
    void SetShortcuts();
    void HandleUndo();
    void HandleRedo();
    void InitializeLayers();
    void SetupLayerUI();
    void SetupLayerDockWidget();

    CommandManager *commandManager;
    LayerManager *layerManager;
    LayerView* layerView;
    LayerWidget* layerWidget;
    void onLayersChanged();
};
#endif // MAINWINDOW_H
