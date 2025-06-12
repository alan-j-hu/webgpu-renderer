#ifndef REDUCER_H
#define REDUCER_H

#include "noworry/material/texturematerial.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class TileRotations;

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
    std::optional<TileRotations*> mesh;
    RotationTag rotation;
    std::optional<TextureRef> texture;
};

/// A TileInst instances a tile definition on the map.
struct TileInst
{
public:
    TileInst(std::shared_ptr<TileDef>, int);

    const TileDef& def() const { return *m_def; }
    int z() const { return m_z; }

private:
    std::shared_ptr<TileDef> m_def;
    short m_z;
};

/// A Layer is a square grid of TileInsts.
struct Layer
{
public:
    Layer();

    const std::optional<TileInst>& at(int x, int y) const;

    void set(int x, int y, std::optional<TileInst> option);

private:
    std::vector<std::optional<TileInst>> m_tiles;
};

/// A top-level Project.
struct Project
{
    std::size_t tiledef_count() const;
    std::shared_ptr<TileDef> tiledef_at(int idx) const;

    void add_tiledef(TileDef tiledef);
    void remove_tiledef(int idx);

    std::size_t layer_count() const;
    Layer& layer_at(int idx);
    const std::vector<Layer>& layers() const { return m_layers; }

    void add_layer();
    void remove_layer(int idx);

private:
    std::vector<std::shared_ptr<TileDef>> m_tile_defs;
    std::vector<Layer> m_layers;
};

#endif
