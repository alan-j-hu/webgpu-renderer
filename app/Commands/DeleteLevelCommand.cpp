#include "DeleteLevelCommand.h"

DeleteLevelCommand::DeleteLevelCommand(World& world, Level& level)
    : m_world(world), m_level_ref(level)
{
}

const char* DeleteLevelCommand::name()
{
    return "Delete Level";
}


auto DeleteLevelCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    m_level_ptr = m_world.remove_level(m_level_ref.x(), m_level_ref.y());
    return Command::Outcome::DONE;
}

void DeleteLevelCommand::down(Project&)
{
    World::InsertionInfo info {
        m_level_ref.x(),
        m_level_ref.y(),
        "",
        std::move(m_level_ptr)
    };

    m_world.insert_level(std::move(info));
}
