#include "CreateLayerCommand.h"

CreateLayerCommand::CreateLayerCommand(Level& level)
    : CreateLayerCommand(level, level.layer_count())
{
}

CreateLayerCommand::CreateLayerCommand(Level& level, int idx)
    : m_level(level), m_idx(idx), m_layer(std::make_unique<Layer>())
{
}

Command::Outcome CreateLayerCommand::up(Project& project)
{
    m_level.add_layer(std::move(m_layer), m_idx);
    return Outcome::DONE;
}

void CreateLayerCommand::down(Project& project)
{
    m_layer = m_level.remove_layer(m_idx);
}

const char* CreateLayerCommand::name()
{
    return "Create Layer";
}
