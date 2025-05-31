#ifndef TILESET_TILESET_H
#define TILESET_TILESET_H

#include "tiledefinition.h"
#include <vector>

class Tileset
{
public:

    int tile_count() const;

    const TileDefinition& tile_at(int idx) const;
    TileDefinition& tile_at(int idx);

    void add_tile(TileDefinition definition);

private:
    std::vector<TileDefinition> m_tile_definitions;
};

#endif
