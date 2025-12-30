#include "PlaceTileCommand.h"

PlaceTileCommand::PlaceTileCommand(
    LayerLocation loc,
    int x,
    int y,
    int z,
    Rotation rotation,
    std::shared_ptr<TileDef> tiledef)
    : m_loc(loc), m_x(x), m_y(y),
      m_inst(std::make_optional<TileInst>(tiledef, z, rotation))
{
}

Command::Outcome PlaceTileCommand::up(Project& project)
{
    Layer& layer = project.layer_at(m_loc);
    std::optional<TileInst> old = layer.at(m_x, m_y);
    if (m_inst == old) {
        return Outcome::CANCELED;
    }
    layer.set(m_x, m_y, m_inst);
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
