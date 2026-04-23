#include "CreateTiledefCommand.h"
#include <utility>

CreateTiledefCommand::CreateTiledefCommand(TileDef tiledef, int index)
    : m_tiledef(std::move(tiledef)), m_index(index)
{
}

const char* CreateTiledefCommand::undo_string()
{
    return "Undo create tile definition";
}

const char* CreateTiledefCommand::redo_string()
{
    return "Redo create tile definition";
}

auto CreateTiledefCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    project.tileset_at(0)->add(std::move(m_tiledef));
    return Outcome::DONE;
}

void CreateTiledefCommand::down(Project& project)
{
    auto tiledef = project.tileset_at(0)->remove(m_index);
    m_tiledef = std::move(*tiledef);
}
