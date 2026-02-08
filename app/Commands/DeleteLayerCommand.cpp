#include "DeleteLayerCommand.h"

DeleteLayerCommand::DeleteLayerCommand(Level& level, int idx)
    : m_level(level), m_idx(idx)
{
}

const char* DeleteLayerCommand::name()
{
    return "Delete Layer";
}

auto DeleteLayerCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    m_layer = m_level.remove_layer(m_idx);
    if (m_layer == nullptr) {
        return Outcome::UNCHANGED;
    }
    return Outcome::DONE;
}

void DeleteLayerCommand::down(Project& project)
{
    m_level.add_layer(std::move(m_layer), m_idx);
}
