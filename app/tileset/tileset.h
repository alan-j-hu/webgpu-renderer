#ifndef TILESET_TILESET_H
#define TILESET_TILESET_H

#include "tiledefinition.h"
#include <vector>

class Tileset
{
public:
    void add_tile(TileDefinition);

private:
    std::vector<TileDefinition> m_tiles;
};

#endif
