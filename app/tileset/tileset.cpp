#include "tileset.h"
#include <utility>

void Tileset::add_tile(TileDefinition tile)
{
    m_tiles.push_back(std::move(tile));
}
