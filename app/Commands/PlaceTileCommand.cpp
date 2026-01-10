#include "PlaceTileCommand.h"

PlaceTileCommand::PlaceTileCommand(
    Layer& layer,
    int x,
    int y,
    int z,
    Rotation rotation,
    std::shared_ptr<const TileDef> tiledef)
    : m_layer(layer), m_x(x), m_y(y),
      m_inst(std::make_optional<TileInst>(tiledef, z, rotation))
{
}

Command::Outcome PlaceTileCommand::up(Project& project)
{
    std::optional<TileInst> old = m_layer.at(m_x, m_y);
    if (m_inst == old) {
        return Outcome::CANCELED;
    }
    m_layer.set(m_x, m_y, m_inst);
    m_inst = old;
    return Outcome::COMPLETED;
}

void PlaceTileCommand::down(Project& project)
{
    up(project);
}

const char* PlaceTileCommand::name()
{
    return "Place Tile";
}
