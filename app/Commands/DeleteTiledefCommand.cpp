#include "DeleteTiledefCommand.h"

DeleteTiledefCommand::DeleteTiledefCommand(Tileset& tileset, int index)
    : m_index(index), m_tileset(&tileset)
{
}

const char* DeleteTiledefCommand::undo_string()
{
    return "Undo delete tile definition";
}

const char* DeleteTiledefCommand::redo_string()
{
    return "Redo delete tile definition";
}

auto DeleteTiledefCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    m_tiledef = m_tileset->at(m_index);
    if (project.tiledef_in_use(*m_tiledef)) {
        return std::unexpected(
           "Tile definition has uses in a map, and cannot be deleted.");
    }

    m_tileset->remove(m_index);
    return Outcome::DONE;
}

void DeleteTiledefCommand::down(Project& project)
{
    m_tileset->add(std::move(*m_tiledef), m_index);
    m_tiledef.reset();
}
