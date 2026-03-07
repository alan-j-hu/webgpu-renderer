#include "MoveTiledefCommand.h"

MoveTiledefCommand::MoveTiledefCommand(
    Tileset& tileset,
    int old_idx,
    int new_idx)
    : m_tileset(&tileset), m_old_idx(old_idx), m_new_idx(new_idx)
{
}

auto MoveTiledefCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    if (m_old_idx == m_new_idx) {
        return Outcome::UNCHANGED;
    }

    m_tileset->move(m_old_idx, m_new_idx);
    return Outcome::DONE;
}

void MoveTiledefCommand::down(Project& project)
{
    m_tileset->move(m_new_idx, m_old_idx);
}

const char* MoveTiledefCommand::name()
{
    return "move tile definition";
}
