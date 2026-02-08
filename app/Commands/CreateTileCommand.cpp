#include "CreateTileCommand.h"
#include <utility>

CreateTileCommand::CreateTileCommand(TileDef tile_def, int index)
    : m_tile_def(std::move(tile_def)), m_index(index)
{
}

auto CreateTileCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    project.tileset_at(0)->add(m_tile_def);
    return Outcome::DONE;
}

void CreateTileCommand::down(Project& project)
{
    project.tileset_at(0)->remove(m_index);
}

const char* CreateTileCommand::name()
{
    return "Create Tile";
}
