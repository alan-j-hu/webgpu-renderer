#ifndef TILESET_TILEDEFINITION_H
#define TILESET_TILEDEFINITION_H

#include "rotationdropdown.h"
#include "tilerotations.h"
#include "noworry/material/texturematerial.h"
#include <memory>

struct TileDefinition
{
    TileRotations* selected_mesh;
    RotationTag rotation;
    std::shared_ptr<TextureMaterial> material;

    TileDefinition();
    TileDefinition(const TileDefinition&) = default;
    TileDefinition& operator=(const TileDefinition&) = default;
    TileDefinition(TileDefinition&&) = default;
    TileDefinition& operator=(TileDefinition&&) = default;

    TileMesh& tile_mesh() { return selected_mesh->rotated(rotation); }
};

#endif
