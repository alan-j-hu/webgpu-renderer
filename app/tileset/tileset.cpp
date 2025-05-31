#include "tileset.h"
#include <utility>

int Tileset::tile_count() const
{
    return m_tile_definitions.size();
}

const TileDefinition& Tileset::tile_at(int idx) const
{
    return m_tile_definitions.at(idx);
}

TileDefinition& Tileset::tile_at(int idx)
{
    return m_tile_definitions.at(idx);
}

void Tileset::add_tile(TileDefinition tile)
{
    m_tile_definitions.push_back(std::move(tile));
}
