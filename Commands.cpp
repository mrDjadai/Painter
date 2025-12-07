// LayerCommands.cpp
#include "Commands.h"
#include "LayerManager.h"

// AddLayerCommand
AddLayerCommand::AddLayerCommand(LayerManager* manager, const QSize& size, const QString& name)
    : manager(manager), size(size), name(name), createdLayer(nullptr), layerIndex(-1)
{
}

AddLayerCommand::~AddLayerCommand()
{
    // Память управляется LayerManager
}

void AddLayerCommand::Do()
{
    if (manager) {
        createdLayer = manager->createNewLayer(size, name);
        layerIndex = manager->layerCount() - 1;
    }
}

void AddLayerCommand::Undo()
{
    if (manager && layerIndex >= 0) {
        manager->removeLayer(layerIndex);
        createdLayer = nullptr;
    }
}

void AddLayerCommand::Redo()
{
    if (manager && createdLayer) {
        // LayerManager создаст новый слой, но нам нужно восстановить содержимое
        Layer* newLayer = manager->createNewLayer(size, name);
        if (newLayer) {
            newLayer->setImage(createdLayer->image().copy());
            newLayer->setOpacity(createdLayer->opacity());
            newLayer->setVisible(createdLayer->isVisible());
        }
        layerIndex = manager->layerCount() - 1;
    }
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

// RenameLayerCommand
RenameLayerCommand::RenameLayerCommand(LayerManager* manager, int layerIndex, const QString& newName)
    : manager(manager), layerIndex(layerIndex), newName(newName)
{
    if (manager) {
        Layer* layer = manager->layerAt(layerIndex);
        if (layer) {
            oldName = layer->name();
        }
    }
}

void RenameLayerCommand::Do()
{
    if (manager) {
        Layer* layer = manager->layerAt(layerIndex);
        if (layer) {
            layer->setName(newName);
            manager->layersChanged();
        }
    }
}

void RenameLayerCommand::Undo()
{
    if (manager) {
        Layer* layer = manager->layerAt(layerIndex);
        if (layer) {
            layer->setName(oldName);
            manager->layersChanged();
        }
    }
}

void RenameLayerCommand::Redo()
{
    Do();
}

// DuplicateLayerCommand
DuplicateLayerCommand::DuplicateLayerCommand(LayerManager* manager, int layerIndex)
    : manager(manager), sourceIndex(layerIndex), duplicatedLayer(nullptr), duplicateIndex(-1)
{
}

void DuplicateLayerCommand::Do()
{
    if (manager) {
        manager->duplicateLayer(sourceIndex);
        duplicateIndex = manager->layerCount() - 1;
        duplicatedLayer = manager->layerAt(duplicateIndex);
    }
}

void DuplicateLayerCommand::Undo()
{
    if (manager && duplicateIndex >= 0) {
        manager->removeLayer(duplicateIndex);
    }
}

void DuplicateLayerCommand::Redo()
{
    if (manager && duplicatedLayer) {
        // Создаем дубликат с тем же содержимым
        Layer* newLayer = manager->createNewLayer(duplicatedLayer->image().size(),
                                                  duplicatedLayer->name() + " Copy");
        if (newLayer) {
            newLayer->setImage(duplicatedLayer->image().copy());
            newLayer->setOpacity(duplicatedLayer->opacity());
            newLayer->setVisible(duplicatedLayer->isVisible());
        }
        duplicateIndex = manager->layerCount() - 1;
    }
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
    m_layerManager->layersChanged();
}

void DrawCommand::Undo()
{
    if (!m_layerManager) return;

    Layer* layer = m_layerManager->layerAt(m_layerIndex);
    if (!layer) return;

    layer->setImage(m_beforeImage);
    m_layerManager->layersChanged();
}

void DrawCommand::Redo()
{
    Do();
}
