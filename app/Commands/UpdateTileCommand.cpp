#include "UpdateTileCommand.h"

UpdateTileCommand::UpdateTileCommand(int idx, TileDef tile_def)
    : m_idx(idx), m_tiledef(std::move(tile_def))
{
}

void UpdateTileCommand::up(Project&)
{
}

void UpdateTileCommand::down(Project&)
{
}

const char* UpdateTileCommand::name()
{
    return "Update Tile";
}
