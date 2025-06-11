#ifndef REDUCER_H
#define REDUCER_H

#include "noworry/material/texturematerial.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <immer/vector.hpp>

enum class RotationTag
{
    Rotate0,
    Rotate90,
    Rotate180,
    Rotate270
};

/// A TextureRef consists of a file path and a material for the texture.
struct TextureRef
{
    std::filesystem::path path;
    std::shared_ptr<TextureMaterial> material;
};

/// A TileDefinition has a mesh, rotation, and texture and is referenced by
/// TileInsts.
struct TileDef
{
    std::optional<std::shared_ptr<std::string>> mesh;
    RotationTag rotation;
    std::optional<TextureRef> texture;
};

/// A TileInst instances a tile definition on the map.
struct TileInst
{
    int def;
    short z;
};

/// A Layer is a square grid of TileInsts.
struct Layer
{
public:
    Layer();

    const std::optional<TileInst>& at(int x, int y) const;

    Layer set(int x, int y, std::optional<TileInst> option) const;

private:
    immer::vector<std::optional<TileInst>> m_tiles;
};

/// A top-level Project.
struct Project
{
    const immer::vector<TileDef>& tile_defs() const { return m_tile_defs; }
    const immer::vector<Layer>& layers() const { return m_layers; }

    Project with_tile_defs(immer::vector<TileDef>) const;

    template<class F>
    Project map_tile_defs(F f) const
    {
        return with_tile_defs(f(m_tile_defs));
    }

    Project with_layers(immer::vector<Layer>) const;

    template<class F>
    Project map_layers(F f) const
    {
        return with_layers(f(m_layers));
    }

private:
    immer::vector<TileDef> m_tile_defs;
    immer::vector<Layer> m_layers;
};

#endif
