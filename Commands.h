#ifndef COMMANDS_H
#define COMMANDS_H

#include "CommandSystem.h"
#include "Layer.h"
#include <QList>
#include <QPointer>
#include "LayerWidget.h"

class AddLayerCommand : public Command
{
public:
    AddLayerCommand(LayerManager* manager, const QSize& size, const QString& name = "New Layer");
    ~AddLayerCommand();

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    QPointer<LayerManager> manager;
    QSize size;
    QString name;
    Layer* createdLayer;
    int layerIndex;
};

// Команда удаления слоя
class DeleteLayerCommand : public Command
{
public:
    DeleteLayerCommand(LayerManager* manager, int layerIndex);
    ~DeleteLayerCommand();

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    QPointer<LayerManager> manager;
    int layerIndex;
    std::unique_ptr<Layer> deletedLayer;
    bool wasActive;
};

// Команда перемещения слоя
class MoveLayerCommand : public Command
{
public:
    MoveLayerCommand(LayerManager* manager, int fromIndex, int toIndex);

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    QPointer<LayerManager> manager;
    int fromIndex;
    int toIndex;
};

// Команда изменения видимости слоя
class ToggleLayerVisibilityCommand : public Command
{
public:
    ToggleLayerVisibilityCommand(LayerManager* manager, int layerIndex);

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    QPointer<LayerManager> manager;
    int layerIndex;
    bool oldVisibility;
};

// Команда изменения непрозрачности слоя
class ChangeLayerOpacityCommand : public Command
{
public:
    ChangeLayerOpacityCommand(LayerManager* manager, int layerIndex, float newOpacity);

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    QPointer<LayerManager> manager;
    int layerIndex;
    float newOpacity;
    float oldOpacity;
};

// Команда переименования слоя
class RenameLayerCommand : public Command
{
public:
    RenameLayerCommand(LayerManager* manager, int layerIndex, const QString& newName);

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    QPointer<LayerManager> manager;
    int layerIndex;
    QString newName;
    QString oldName;
};

// Команда дублирования слоя
class DuplicateLayerCommand : public Command
{
public:
    DuplicateLayerCommand(LayerManager* manager, int layerIndex);

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    QPointer<LayerManager> manager;
    int sourceIndex;
    Layer* duplicatedLayer;
    int duplicateIndex;
};

#endif // COMMANDS_H
