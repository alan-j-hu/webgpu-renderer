#include "UpdateTiledefCommand.h"

UpdateTiledefCommand::UpdateTiledefCommand(int idx, TileDef tiledef)
    : m_idx(idx), m_tiledef(std::move(tiledef))
{
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

const char* UpdateTiledefCommand::name()
{
    return "update tile definition";
}
