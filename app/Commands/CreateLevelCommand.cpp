#include "CreateLevelCommand.h"

CreateLevelCommand::CreateLevelCommand(World& world, int x, int y)
    : m_world(&world), m_level(std::make_unique<Level>()), m_x(x), m_y(y)
{
}

const char* CreateLevelCommand::undo_string()
{
    return "Undo create level";
}

const char* CreateLevelCommand::redo_string()
{
    return "Redo create level";
}

auto CreateLevelCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    if (m_world->insert_level(m_x, m_y, std::move(m_level))) {
        return Outcome::DONE;
    }

    return Outcome::UNCHANGED;
}

void CreateLevelCommand::down(Project& project)
{
    m_level = m_world->remove_level(m_x, m_y);
}
