#include "CreateTileCommand.h"
#include <utility>

CreateTileCommand::CreateTileCommand(TileDef tile_def)
    : m_tile_def(std::move(tile_def))
{
}

Command::Outcome CreateTileCommand::up(Project& project)
{
    project.add_tiledef(m_tile_def);
    return Outcome::COMPLETED;
}

void CreateTileCommand::down(Project& project)
{
    project.remove_tiledef(project.tiledef_count() - 1);
}

const char* CreateTileCommand::name()
{
    return "Create Tile";
}
