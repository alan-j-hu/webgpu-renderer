#include "CreateTileCommand.h"
#include <utility>

CreateTileCommand::CreateTileCommand(TileDef tile_def)
    : m_tile_def(std::move(tile_def))
{
}

Command::Outcome CreateTileCommand::up(Project& project)
{
    project.tileset_at(0)->add(m_tile_def);
    return Outcome::DONE;
}

void CreateTileCommand::down(Project& project)
{
    project.tileset_at(0)->remove(project.tileset_at(0)->count() - 1);
}

const char* CreateTileCommand::name()
{
    return "Create Tile";
}
