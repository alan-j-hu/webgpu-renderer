#include "ResizeWorldCommand.h"

ResizeWorldCommand::ResizeWorldCommand(
    World& world,
    int top,
    int bottom,
    int left,
    int right)
    : m_world(&world),
      m_top(top),
      m_bottom(bottom),
      m_left(left),
      m_right(right)
{
}

const char* ResizeWorldCommand::undo_string()
{
    return "Undo resize world";
}

const char* ResizeWorldCommand::redo_string()
{
    return "Redo resize world";
}

auto ResizeWorldCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    m_world->expand(m_top, m_bottom, m_left, m_right);
    return Outcome::DONE;
}

void ResizeWorldCommand::down(Project& project)
{
    m_world->expand(-m_top, -m_bottom, -m_left, -m_right);
}
