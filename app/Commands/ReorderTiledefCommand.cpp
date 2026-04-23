#include "ReorderTiledefCommand.h"

ReorderTiledefCommand::ReorderTiledefCommand(
    Tileset& tileset,
    int idx,
    int delta)
    : m_tileset(&tileset), m_idx(idx), m_delta(delta)
{
}

const char* ReorderTiledefCommand::undo_string()
{
    return "Undo move tile definition";
}

const char* ReorderTiledefCommand::redo_string()
{
    return "Redo move tile definition";
}

auto ReorderTiledefCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    if (m_delta == 0) {
        return Outcome::UNCHANGED;
    }

    m_tileset->move(m_idx, m_delta);
    return Outcome::DONE;
}

void ReorderTiledefCommand::down(Project& project)
{
    m_tileset->move(m_idx + m_delta, -m_delta);
}
