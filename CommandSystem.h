#ifndef COMMANDSYSTEM_H
#define COMMANDSYSTEM_H

#include <vector>

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
    std::vector<Command*> undoStack;
    std::vector<Command*> redoStack;

public:
    void ExecuteCommand(Command* command);
    bool Undo();
    bool Redo();
    bool CanUndo();
    bool CanRedo();
    void Clear();
};

#endif // COMMANDSYSTEM_H
