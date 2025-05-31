#ifndef TILESET_TILEDEFINITION_H
#define TILESET_TILEDEFINITION_H

#include "tilemesh.h"

class TileDefinition
{
public:
    TileDefinition(const TileMesh&);

    const TileMesh& tilemesh() const { return *m_tilemesh; }

private:
    const TileMesh* m_tilemesh;
};

#endif
