#ifndef COMMAND_H
#define COMMAND_H

#include "../Reducer.h"

/// Abstract class for undo/redo functionality.
class Command
{
public:
    Command();

    virtual const char* name() = 0;

    /// Throws if redo is attempted twice in a row.
    void redo(Project&);
    /// Throws if undo is attempted twice in a row.
    void undo(Project&);

protected:
    /// Override to implement the logic for redo.
    virtual void up(Project&) = 0;
    /// Override to implement the logic for undo.
    virtual void down(Project&) = 0;

private:
    enum class State {
        DOWN,
        UP
    } m_state;
};

#endif
