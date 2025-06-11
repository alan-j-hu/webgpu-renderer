#include "reducer.h"

Layer::Layer()
{
    for (int i = 0; i < 16 * 16; ++i) {
        m_tiles = m_tiles.push_back(std::nullopt);
    }
}

const std::optional<TileInst>& Layer::at(int x, int y) const
{
    return m_tiles.at(y * 16 + x);
}

Layer Layer::set(int x, int y, std::optional<TileInst> option) const
{
    Layer new_layer = *this;
    new_layer.m_tiles = m_tiles.set(y * 16 + x, option);
    return new_layer;
}

Project Project::with_tile_defs(immer::vector<TileDef> tile_defs) const
{
    Project new_project = *this;
    new_project.m_tile_defs = tile_defs;
    return new_project;
}

Project Project::with_layers(immer::vector<Layer> layers) const
{
    Project new_project = *this;
    new_project.m_layers = layers;
    return new_project;
}
