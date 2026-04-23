#include "ReorderLayerCommand.h"

ReorderLayerCommand::ReorderLayerCommand(
    Level& level,
    int idx,
    int delta)
    : m_level(&level), m_idx(idx), m_delta(delta)
{
}

const char* ReorderLayerCommand::undo_string()
{
    return "Undo move layer";
}

const char* ReorderLayerCommand::redo_string()
{
    return "Redo move layer";
}

auto ReorderLayerCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    if (m_delta == 0) {
        return Outcome::UNCHANGED;
    }

    m_level->move_layer(m_idx, m_delta);
    return Outcome::DONE;
}

void ReorderLayerCommand::down(Project& project)
{
    m_level->move_layer(m_idx + m_delta, -m_delta);
}
