#include "CreateLayerCommand.h"

CreateLayerCommand::CreateLayerCommand(Level& level)
    : CreateLayerCommand(level, level.layer_count())
{
}

CreateLayerCommand::CreateLayerCommand(Level& level, int idx)
    : m_level(level), m_idx(idx), m_layer(std::make_unique<Layer>())
{
}

const char* CreateLayerCommand::undo_string()
{
    return "Undo create layer";
}

const char* CreateLayerCommand::redo_string()
{
    return "Redo create layer";
}

auto CreateLayerCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    m_level.add_layer(std::move(m_layer), m_idx);
    return Outcome::DONE;
}

void CreateLayerCommand::down(Project& project)
{
    m_layer = m_level.remove_layer(m_idx);
}
