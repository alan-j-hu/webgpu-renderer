#include "DeleteLayerCommand.h"

DeleteLayerCommand::DeleteLayerCommand(int index)
    : m_index(index)
{
}

const char* DeleteLayerCommand::name()
{
    return "Delete Layer";
}

Command::Outcome DeleteLayerCommand::up(Project& project)
{
    m_layer = project.remove_layer(m_index);
    if (m_layer == nullptr) {
        return Outcome::CANCELED;
    }
    return Outcome::COMPLETED;
}

void DeleteLayerCommand::down(Project& project)
{
    project.add_layer(std::move(*m_layer), m_index);
    m_layer = std::nullopt;
}
