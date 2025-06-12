#include "placetilecommand.h"

PlaceTileCommand::PlaceTileCommand(
    int idx, int x, int y, int z, std::shared_ptr<TileDef> def)
    : m_layer_idx(idx), m_x(x), m_y(y),
      m_inst(std::make_optional<TileInst>(def, z))
{
}

void PlaceTileCommand::up(Project& project)
{
    Layer& layer = project.layer_at(m_layer_idx);
    std::optional<TileInst> old = layer.at(m_x, m_y);
    layer.set(m_x, m_y, m_inst);
    m_inst = old;
}

void PlaceTileCommand::down(Project& project)
{
    up(project);
}

const char* PlaceTileCommand::name()
{
    return "Place Tile";
}
