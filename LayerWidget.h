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
    void SetRow(int row);

private slots:
    void onAddLayerClicked();
    void onRemoveLayerClicked();
    void onDuplicateLayerClicked();
    void onLayerSelectionChanged();
    void onLayerVisibilityChanged(int realIndex, bool visible);
    void onLayerMoved(int fromIndex, int toIndex);


    void updateLayerList();
    void onOpacitySliderPressed();
    void onOpacitySliderValueChanged(int value);   // для мгновенного обновления прозрачности
    void onOpacitySliderReleased();                // для добавления команды в CommandManager
    void updateOpacitySlider();
    void onRenameLayerClicked();
    void onMergeWithNextClicked();

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
    QToolButton* m_renameButton;
    QToolButton* m_mergeButton;

    QSlider* m_opacitySlider;
    float m_startOpacity = 1.0f;
};

#endif // LAYERWIDGET_H
