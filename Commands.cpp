// LayerCommands.cpp
#include "Commands.h"
#include "LayerManager.h"
#include <qpainter.h>


AddLayerCommand::AddLayerCommand(LayerManager* manager,
                                 const QSize& size,
                                 const QString& name)
    : manager(manager)
    , m_size(size)
    , m_name(name)
    , m_index(-1)
{
}

void AddLayerCommand::Do()
{
    if (!manager) return;

    Layer* layer = manager->createNewLayer(m_size, m_name);
    if (!layer) return;

    m_index = manager->layerCount() - 1;

    // Сохраняем параметры, чтобы Redo был корректным
    m_image = layer->image().copy();
    m_opacity = layer->opacity();
    m_visibility = layer->isVisible();

    manager->setActiveLayer(m_index);
}

void AddLayerCommand::Undo()
{
    if (!manager || m_index < 0) return;

    manager->removeLayer(m_index);
}

void AddLayerCommand::Redo()
{
    if (!manager) return;

    // создаём снова
    Layer* newLayer = manager->createNewLayer(m_size, m_name);
    if (!newLayer) return;

    newLayer->setImage(m_image.copy());
    newLayer->setOpacity(m_opacity);
    newLayer->setVisible(m_visibility);

    // index станет последним
    m_index = manager->layerCount() - 1;

    manager->setActiveLayer(m_index);
}

// DeleteLayerCommand
DeleteLayerCommand::DeleteLayerCommand(LayerManager* manager, int layerIndex)
    : manager(manager), layerIndex(layerIndex), wasActive(false)
{
    if (manager && layerIndex >= 0 && layerIndex < manager->layerCount()) {
        Layer* layer = manager->layerAt(layerIndex);
        if (layer) {
            // Сохраняем копию слоя
            deletedLayer = std::make_unique<Layer>(layer->image().size(), layer->name());
            deletedLayer->setImage(layer->image().copy());
            deletedLayer->setOpacity(layer->opacity());
            deletedLayer->setVisible(layer->isVisible());

            // Запоминаем, был ли это активный слой
            wasActive = (manager->activeLayerIndex() == layerIndex);
        }
    }
}

DeleteLayerCommand::~DeleteLayerCommand()
{
}

void DeleteLayerCommand::Do()
{
    if (manager && layerIndex >= 0 && layerIndex < manager->layerCount()) {
        manager->removeLayer(layerIndex);
    }
}

void DeleteLayerCommand::Undo()
{
    if (manager && deletedLayer) {
        // Вставляем сохраненный слой обратно
        auto layer = std::make_unique<Layer>(deletedLayer->image().size(), deletedLayer->name());
        layer->setImage(deletedLayer->image().copy());
        layer->setOpacity(deletedLayer->opacity());
        layer->setVisible(deletedLayer->isVisible());

        manager->insertLayer(layerIndex, std::move(layer));

        // Восстанавливаем активный слой если нужно
        if (wasActive) {
            manager->setActiveLayer(layerIndex);
        }
    }
}

void DeleteLayerCommand::Redo()
{
    Do();
}

// MoveLayerCommand
MoveLayerCommand::MoveLayerCommand(LayerManager* manager, int fromIndex, int toIndex)
    : manager(manager), fromIndex(fromIndex), toIndex(toIndex)
{
}

void MoveLayerCommand::Do()
{
    if (manager) {
        manager->moveLayer(fromIndex, toIndex);
    }
}

void MoveLayerCommand::Undo()
{
    if (manager) {
        manager->moveLayer(toIndex, fromIndex);
    }
}

void MoveLayerCommand::Redo()
{
    Do();
}

// ToggleLayerVisibilityCommand
ToggleLayerVisibilityCommand::ToggleLayerVisibilityCommand(LayerManager* manager, int layerIndex)
    : manager(manager), layerIndex(layerIndex), oldVisibility(true)
{
    if (manager) {
        Layer* layer = manager->layerAt(layerIndex);
        if (layer) {
            oldVisibility = layer->isVisible();
        }
    }
}

void ToggleLayerVisibilityCommand::Do()
{
    if (manager) {
        Layer* layer = manager->layerAt(layerIndex);
        if (layer) {
            layer->setVisible(!layer->isVisible());
            manager->layersChanged();
        }
    }
}

void ToggleLayerVisibilityCommand::Undo()
{
    if (manager) {
        Layer* layer = manager->layerAt(layerIndex);
        if (layer) {
            layer->setVisible(oldVisibility);
            manager->layersChanged();
        }
    }
}

void ToggleLayerVisibilityCommand::Redo()
{
    Do();
}

// ChangeLayerOpacityCommand
ChangeLayerOpacityCommand::    ChangeLayerOpacityCommand(LayerManager* m, int layerIndex, float oldOpacity, float newOpacity)
    : manager(m)
    , layerIndex(layerIndex)
    , oldOpacity(oldOpacity)
    , newOpacity(newOpacity)
{}

void ChangeLayerOpacityCommand::Do()
{
    if (manager) {
        Layer* layer = manager->layerAt(layerIndex);
        if (layer) {
            layer->setOpacity(newOpacity);
            manager->layersChanged();
        }
    }
}

void ChangeLayerOpacityCommand::Undo()
{
    if (manager) {
        Layer* layer = manager->layerAt(layerIndex);
        if (layer) {
            layer->setOpacity(oldOpacity);
            manager->layersChanged();
        }
    }
}

void ChangeLayerOpacityCommand::Redo()
{
    Do();
}


DrawCommand::DrawCommand(LayerManager* manager, int layerIndex, const QImage& before, const QImage& after)
    : m_layerManager(manager)
    , m_layerIndex(layerIndex)
    , m_beforeImage(before)
    , m_afterImage(after)
{
}

void DrawCommand::Do()
{
    if (!m_layerManager) return;

    Layer* layer = m_layerManager->layerAt(m_layerIndex);
    if (!layer) return;

    layer->setImage(m_afterImage);
    m_layerManager->setActiveLayer(m_layerIndex);
    m_layerManager->layersChanged();
}

void DrawCommand::Undo()
{
    if (!m_layerManager) return;

    Layer* layer = m_layerManager->layerAt(m_layerIndex);
    if (!layer) return;

    layer->setImage(m_beforeImage);
    m_layerManager->setActiveLayer(m_layerIndex);
    m_layerManager->layersChanged();
}

void DrawCommand::Redo()
{
    Do();
}

RenameLayerCommand::RenameLayerCommand(LayerManager* manager, int index,
                                       const QString& oldName, const QString& newName)
    : m_manager(manager)
    , m_index(index)
    , m_oldName(oldName)
    , m_newName(newName)
{
}

void RenameLayerCommand::Do()
{
    Redo();
}

void RenameLayerCommand::Undo()
{
    if (!m_manager) return;

    Layer* layer = m_manager->layerAt(m_index);
    if (!layer) return;

    layer->setName(m_oldName);
    m_manager->layersChanged();
}

void RenameLayerCommand::Redo()
{
    if (!m_manager) return;

    Layer* layer = m_manager->layerAt(m_index);
    if (!layer) return;

    layer->setName(m_newName);
    m_manager->layersChanged();
}

MergeLayerWithNextCommand::MergeLayerWithNextCommand(LayerManager* manager, int topIndex)
    : m_manager(manager)
    , m_topIndex(topIndex)
    , m_topBackup(*m_manager->layerAt(topIndex))
    , m_bottomBackup(*m_manager->layerAt(topIndex - 1))
{
}

void MergeLayerWithNextCommand::Do()
{
    Redo();
}

void MergeLayerWithNextCommand::Redo()
{
    if (!m_manager) return;

    int bottomIndex = m_topIndex - 1;
    if (bottomIndex < 0) return;

    Layer* top = m_manager->layerAt(m_topIndex);
    Layer* bottom = m_manager->layerAt(bottomIndex);
    if (!top || !bottom) return;

    // Рисуем верхний слой поверх нижнего
    QPainter p(&bottom->image());
    p.setOpacity(top->opacity());
    p.drawImage(0, 0, top->image());
    p.end();

    // Удаляем верхний слой
    m_manager->removeLayer(m_topIndex);

    // Обновляем UI
    m_manager->layersChanged();
}

void MergeLayerWithNextCommand::Undo()
{
    if (!m_manager) return;

    // Удаляем объединённый слой (на позиции верхнего)
    m_manager->removeLayer(m_topIndex - 1);

    // Вставляем обратно исходные слои в правильном порядке
    // Сначала нижний слой (м_topIndex - 1), потом верхний (м_topIndex)
    m_manager->insertLayer(m_topIndex - 1, std::make_unique<Layer>(m_bottomBackup));
    m_manager->insertLayer(m_topIndex, std::make_unique<Layer>(m_topBackup));

    // Восстанавливаем активный слой
    m_manager->setActiveLayer(m_topIndex);

    m_manager->layersChanged();
}


DuplicateLayerCommand::DuplicateLayerCommand(LayerManager* manager, int layerIndex)
    : m_manager(manager), m_sourceIndex(layerIndex), m_duplicateIndex(-1)
{
}

void DuplicateLayerCommand::Do()
{
    if (!m_manager || m_sourceIndex < 0 || m_sourceIndex >= m_manager->layerCount())
        return;

    const Layer* sourceLayer = m_manager->layerAt(m_sourceIndex);
    if (!sourceLayer)
        return;

    // Создаем копию слоя
    m_duplicatedLayer = std::make_unique<Layer>(sourceLayer->image().size(),
                                                sourceLayer->name() + " Copy");
    m_duplicatedLayer->setImage(sourceLayer->image().copy());
    m_duplicatedLayer->setOpacity(sourceLayer->opacity());
    m_duplicatedLayer->setVisible(sourceLayer->isVisible());

    // Вставляем слой сразу после исходного
    m_duplicateIndex = m_sourceIndex + 1;
    m_manager->insertLayer(m_duplicateIndex, std::move(m_duplicatedLayer));
}

void DuplicateLayerCommand::Undo()
{
    if (!m_manager || m_duplicateIndex < 0 || m_duplicateIndex >= m_manager->layerCount())
        return;

    // Перемещаем слой обратно в уникальный указатель для Redo
    Layer* layer = m_manager->layerAt(m_duplicateIndex);
    if (layer) {
        m_duplicatedLayer = std::make_unique<Layer>(layer->image().size(), layer->name());
        m_duplicatedLayer->setImage(layer->image().copy());
        m_duplicatedLayer->setOpacity(layer->opacity());
        m_duplicatedLayer->setVisible(layer->isVisible());
        m_manager->removeLayer(m_duplicateIndex);
    }
}

void DuplicateLayerCommand::Redo()
{
    if (!m_manager || !m_duplicatedLayer)
        return;

    // Вставляем слой обратно
    m_manager->insertLayer(m_duplicateIndex, std::move(m_duplicatedLayer));
}
