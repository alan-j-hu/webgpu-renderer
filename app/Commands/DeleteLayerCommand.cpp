#include "DeleteLayerCommand.h"

DeleteLayerCommand::DeleteLayerCommand(LayerLocation loc)
    : m_loc(loc)
{
}

const char* DeleteLayerCommand::name()
{
    return "Delete Layer";
}

Command::Outcome DeleteLayerCommand::up(Project& project)
{
    m_layer = project.level_at(m_loc).remove_layer(m_loc.layer);
    if (m_layer == nullptr) {
        return Outcome::CANCELED;
    }
    return Outcome::COMPLETED;
}

void DeleteLayerCommand::down(Project& project)
{
    project.level_at(m_loc).add_layer(std::move(*m_layer), m_loc.layer);
    m_layer = std::nullopt;
}
