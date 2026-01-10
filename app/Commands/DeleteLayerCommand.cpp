#include "DeleteLayerCommand.h"

DeleteLayerCommand::DeleteLayerCommand(Level& level, int idx)
    : m_level(level), m_idx(idx)
{
}

const char* DeleteLayerCommand::name()
{
    return "Delete Layer";
}

Command::Outcome DeleteLayerCommand::up(Project& project)
{
    m_layer = m_level.remove_layer(m_idx);
    if (m_layer == nullptr) {
        return Outcome::CANCELED;
    }
    return Outcome::COMPLETED;
}

void DeleteLayerCommand::down(Project& project)
{
    m_level.add_layer(std::move(m_layer), m_idx);
}
