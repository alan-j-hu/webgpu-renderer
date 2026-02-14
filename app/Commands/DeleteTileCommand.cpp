#include "DeleteTileCommand.h"

DeleteTileCommand::DeleteTileCommand(int index)
    : m_index(index)
{
}

auto DeleteTileCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    m_tiledef = project.tileset_at(0)->at(m_index);
    if (project.tiledef_in_use(*m_tiledef)) {
        return std::unexpected(
           "Tile definition has uses in a map, and cannot be deleted.");
    }

    project.tileset_at(0)->remove(m_index);
    return Outcome::DONE;
}

void DeleteTileCommand::down(Project& project)
{
    project.tileset_at(0)->add(std::move(*m_tiledef), m_index);
    m_tiledef.reset();
}

const char* DeleteTileCommand::name()
{
    return "Delete Tile";
}
