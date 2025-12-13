#ifndef COMMANDS_H
#define COMMANDS_H

#include "CommandSystem.h"
#include "Layer.h"
#include <QList>
#include <QPointer>
#include "LayerManager.h"

class AddLayerCommand : public Command
{
public:
    AddLayerCommand(LayerManager* manager,
                    const QSize& size,
                    const QString& name = "New Layer");

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    QPointer<LayerManager> manager;

    QSize m_size;
    QString m_name;

    int m_index;

    QImage m_image;
    float m_opacity;
    bool  m_visibility;
};

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

class ChangeLayerOpacityCommand : public Command
{
public:
    ChangeLayerOpacityCommand(LayerManager* m, int layerIndex, float oldOpacity, float newOpacity);

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    QPointer<LayerManager> manager;
    int layerIndex;
    float newOpacity;
    float oldOpacity;
};

class DuplicateLayerCommand : public Command
{
public:
    DuplicateLayerCommand(LayerManager* manager, int layerIndex);

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    QPointer<LayerManager> m_manager;
    int m_sourceIndex;
    std::unique_ptr<Layer> m_duplicatedLayer;
    int m_duplicateIndex;
};

class DrawCommand : public Command
{
public:
    DrawCommand(LayerManager* manager, int layerIndex, const QImage& before, const QImage& after);

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    LayerManager* m_layerManager;
    int m_layerIndex;
    QImage m_beforeImage;
    QImage m_afterImage;
};

class RenameLayerCommand : public Command
{
public:
    RenameLayerCommand(LayerManager* manager, int index,
                       const QString& oldName, const QString& newName);

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    LayerManager* m_manager;
    int m_index;
    QString m_oldName;
    QString m_newName;
};


class MergeLayerWithNextCommand : public Command
{
public:
    MergeLayerWithNextCommand(LayerManager* manager, int topIndex);

    void Do() override;
    void Undo() override;
    void Redo() override;

private:
    LayerManager* m_manager;
    int m_topIndex;

    Layer m_topBackup;
    Layer m_bottomBackup;
};



#endif // COMMANDS_H
