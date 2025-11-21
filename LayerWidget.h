#ifndef LAYERWIDGET_H
#define LAYERWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QCheckBox>
#include <QLabel>
#include "LayerManager.h"
#include "CommandSystem.h"

class LayerListWidget : public QListWidget
{
    Q_OBJECT

public:
    LayerListWidget(QWidget* parent = nullptr);

signals:
    void layerMoved(int fromIndex, int toIndex);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    QPoint m_dragStartPosition;
};

class LayerWidget : public QWidget
{
    Q_OBJECT

public:
    LayerWidget(LayerManager* layerManager, CommandManager* comManager, QWidget* parent = nullptr);

private slots:
    void onAddLayerClicked();
    void onRemoveLayerClicked();
    void onDuplicateLayerClicked();
    void onLayerSelectionChanged();
    void onLayerVisibilityChanged(int realIndex, bool visible);
    void onLayerMoved(int fromIndex, int toIndex);
    void onOpacityChanged(int value);

    void updateLayerList();

private:
    void setupUI();
    void setupConnections();
    int getRealLayerIndex(int listIndex) const;
    int getListIndexFromReal(int realIndex) const;

    LayerManager* m_layerManager;
    CommandManager* m_commandManager;

    LayerListWidget* m_layerList;
    QToolButton* m_addButton;
    QToolButton* m_removeButton;
    QToolButton* m_duplicateButton;
    QSlider* m_opacitySlider;
};

#endif // LAYERWIDGET_H
