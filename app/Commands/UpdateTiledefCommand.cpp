#include "UpdateTiledefCommand.h"

UpdateTiledefCommand::UpdateTiledefCommand(int idx, TileDef tiledef)
    : m_idx(idx), m_tiledef(std::move(tiledef))
{
}

const char* UpdateTiledefCommand::undo_string()
{
    return "Undo edit tiledef";
}

const char* UpdateTiledefCommand::redo_string()
{
    return "Redo edit tiledef";
}

auto UpdateTiledefCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    std::shared_ptr<Tileset> tileset = project.tileset_at(0);
    std::shared_ptr<TileDef> old_tiledef = tileset->at(m_idx);
    std::swap(*old_tiledef, m_tiledef);
    return Outcome::DONE;
}

void UpdateTiledefCommand::down(Project& project)
{
    up(project);
}
