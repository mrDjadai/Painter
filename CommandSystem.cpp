#include "CommandSystem.h"

void CommandManager::ExecuteCommand(std::unique_ptr<Command> command)
{
    command->Do();

    undoStack.push_back(std::move(command));

    redoStack.clear();

    if (undoStack.size() > maxStackSize) {
        undoStack.erase(undoStack.begin());
    }
}

bool CommandManager::Undo()
{
    if (undoStack.empty()) {
        return false;
    }

    auto& command = undoStack.back();

    command->Undo();

    redoStack.push_back(std::move(command));
    undoStack.pop_back();

    return true;
}

bool CommandManager::Redo()
{
    if (redoStack.empty()) {
        return false;
    }

    auto& command = redoStack.back();

    command->Redo();

    undoStack.push_back(std::move(command));
    redoStack.pop_back();

    return true;
}

bool CommandManager::CanUndo()
{
    return !undoStack.empty();
}

bool CommandManager::CanRedo()
{
    return !redoStack.empty();
}

void CommandManager::Clear()
{
    undoStack.clear();
    redoStack.clear();
}
