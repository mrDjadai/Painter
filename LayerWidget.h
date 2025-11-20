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

class LayerListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit LayerListWidget(QWidget* parent = nullptr);

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
    explicit LayerWidget(LayerManager* layerManager, QWidget* parent = nullptr);
    void handleLayerMove(int sourceListIndex, int targetListIndex);
private slots:
    void onAddLayerClicked();
    void onRemoveLayerClicked();
    void onDuplicateLayerClicked();
    void onLayerSelectionChanged();
    void onLayerVisibilityChanged(int index, bool visible);
    void updateLayerList();

private:
    void setupUI();
    void setupConnections();
    int getRealLayerIndex(int listIndex) const;
    int getListIndexFromReal(int realIndex) const;

    LayerManager* m_layerManager;
    LayerListWidget* m_layerList;
    QToolButton* m_addButton;
    QToolButton* m_removeButton;
    QToolButton* m_duplicateButton;
    QSlider* m_opacitySlider;
};

#endif // LAYERWIDGET_H
