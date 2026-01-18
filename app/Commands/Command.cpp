#include "Command.h"
#include <stdexcept>

Command::Command()
    : m_state(State::PENDING)
{
}

Command::Outcome Command::first_do(Project& project)
{
    if (m_state != State::PENDING) {
        throw std::logic_error("Command already executed!");
    }
    Command::Outcome outcome = up(project);
    if (outcome == Command::Outcome::DONE) {
        m_state = State::UP;
    }
    return outcome;
}

void Command::finish()
{
    m_state = State::UP;
}

void Command::undo(Project& project)
{
    if (m_state != State::UP) {
        throw std::logic_error("Undo attempted twice!");
    }
    down(project);
    m_state = State::DOWN;
}

void Command::redo(Project& project)
{
    if (m_state != State::DOWN) {
        throw std::logic_error("Redo attempted twice!");
    }
    up(project);
    m_state = State::UP;
}
