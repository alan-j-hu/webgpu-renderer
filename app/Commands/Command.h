#ifndef COMMAND_H
#define COMMAND_H

#include "../Project.h"

/// Abstract class for undo/redo functionality.
class Command
{
public:
    enum class Outcome
    {
        /// The command did not make any changes to the project state.
        UNCHANGED,
        /// The command is still in progress. This value is for actions such
        /// as drawing a stroke, where multiple changes are part of the same
        /// command and should be undone as one unit.
        IN_PROGRESS,
        /// The command has finished, and may be undone.
        DONE
    };

    Command();
    virtual ~Command() = default;

    virtual const char* name() = 0;

    /// Perform the command for the first time. This function may be called
    /// multiple times in a row.
    Outcome first_do(Project&);

    /// Signals that the initial command has finished. Now, the command can
    /// be undone.
    void finish();

    /// Throws if redo is attempted twice in a row.
    void redo(Project&);
    /// Throws if undo is attempted twice in a row.
    void undo(Project&);

protected:
    /// Override to implement the logic for redo.
    virtual Outcome up(Project&) = 0;
    /// Override to implement the logic for undo.
    virtual void down(Project&) = 0;

private:
    enum class State {
        PENDING,
        DOWN,
        UP
    } m_state;
};

#endif
