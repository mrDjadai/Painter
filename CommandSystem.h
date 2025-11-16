#ifndef COMMANDSYSTEM_H
#define COMMANDSYSTEM_H

#include <vector>
#include <memory>

class Command
{
public:
    virtual ~Command() = default;
    virtual void Do() = 0;
    virtual void Undo() = 0;
    virtual void Redo() = 0;
};

class CommandManager
{
private:
    std::vector<std::unique_ptr<Command>> undoStack;
    std::vector<std::unique_ptr<Command>> redoStack;
    size_t maxStackSize = 20;

public:
    void ExecuteCommand(std::unique_ptr<Command> command);
    bool Undo();
    bool Redo();
    bool CanUndo();
    bool CanRedo();
    void Clear();
};

#endif // COMMANDSYSTEM_H
