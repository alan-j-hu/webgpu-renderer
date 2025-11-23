#include "UpdateTileCommand.h"

UpdateTileCommand::UpdateTileCommand(int idx, TileDef tile_def)
    : m_idx(idx), m_tiledef(std::move(tile_def))
{
}

Command::Outcome UpdateTileCommand::up(Project& project)
{
    std::shared_ptr<TileDef> old_tiledef = project.tiledef_at(m_idx);
    project.set_tiledef(m_idx, m_tiledef);
    m_tiledef = *old_tiledef;
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
