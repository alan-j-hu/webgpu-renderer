#include "CreateLevelCommand.h"

CreateLevelCommand::CreateLevelCommand(World& world, int x, int y)
    : m_world(&world), m_level(std::make_unique<Level>()), m_x(x), m_y(y)
{
}

auto CreateLevelCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    World::InsertionInfo info = { m_x, m_y, "level", std::move(m_level) };

    if (m_world->insert_level(std::move(info))) {
        return Outcome::DONE;
    }

    return Outcome::UNCHANGED;
}

void CreateLevelCommand::down(Project& project)
{
    m_level = m_world->remove_level(m_x, m_y);
}

const char* CreateLevelCommand::name()
{
    return "Create Level";
}
