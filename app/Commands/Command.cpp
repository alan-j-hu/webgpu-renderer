#include "Command.h"
#include <stdexcept>

Command::Command()
    : m_state(State::DOWN)
{
}

void Command::undo(Project& project)
{
    if (m_state == State::DOWN) {
        throw std::logic_error("Undo attempted twice!");
    }
    down(project);
    m_state = State::DOWN;
}

Command::Outcome Command::redo(Project& project)
{
    if (m_state == State::UP) {
        throw std::logic_error("Redo attempted twice!");
    }
    Command::Outcome outcome = up(project);
    m_state = State::UP;
    return outcome;
}
