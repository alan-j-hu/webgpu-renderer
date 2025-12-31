#include "UpdateTileCommand.h"

UpdateTileCommand::UpdateTileCommand(
    int idx,
    std::shared_ptr<TileDef> tile_def)
    : m_idx(idx), m_tiledef(std::move(tile_def))
{
}

Command::Outcome UpdateTileCommand::up(Project& project)
{
    std::shared_ptr<Tileset> tileset = project.tileset_at(0);
    std::shared_ptr<TileDef> old_tiledef = tileset->at(m_idx);
    std::swap(old_tiledef, m_tiledef);
    return Outcome::COMPLETED;
}

void UpdateTileCommand::down(Project& project)
{
    up(project);
}

const char* UpdateTileCommand::name()
{
    return "Update Tile";
}
