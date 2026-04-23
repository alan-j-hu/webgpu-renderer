#include "DeleteLevelCommand.h"

DeleteLevelCommand::DeleteLevelCommand(World& world, Level& level)
    : m_world(world), m_level_ref(level)
{
}

const char* DeleteLevelCommand::undo_string()
{
    return "Undo delete level";
}

const char* DeleteLevelCommand::redo_string()
{
    return "Redo delete level";
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
    m_world.insert_level(
        m_level_ref.x(),
        m_level_ref.y(),
        std::move(m_level_ptr));
}
