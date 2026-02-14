#include "CreateTileCommand.h"
#include <utility>

CreateTileCommand::CreateTileCommand(TileDef tiledef, int index)
    : m_tiledef(std::move(tiledef)), m_index(index)
{
}

auto CreateTileCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    project.tileset_at(0)->add(std::move(m_tiledef));
    return Outcome::DONE;
}

void CreateTileCommand::down(Project& project)
{
    auto tiledef = project.tileset_at(0)->remove(m_index);
    m_tiledef = std::move(*tiledef);
}

const char* CreateTileCommand::name()
{
    return "Create Tile";
}
