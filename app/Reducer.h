#ifndef REDUCER_H
#define REDUCER_H

#include "Listener.h"

#include "noworry/Gfx3D/Model.h"
#include "noworry/Material/TextureMaterial.h"

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
    std::optional<std::shared_ptr<ModelData>> model_data;
    std::optional<std::shared_ptr<Model>> model;
    RotationTag rotation;
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

bool operator==(const TileInst&, const TileInst&);

/// A Layer is a square grid of TileInsts.
struct Layer
{
public:
    class Listener
    {
    public:
        virtual void notify(const Layer&) = 0;
    };

    Layer();
    Layer(Layer&&) noexcept = default;
    Layer& operator=(Layer&&) noexcept = default;

    const std::optional<TileInst>& at(int x, int y) const;

    void set(int x, int y, std::optional<TileInst> option);

    Listenable<Listener>& listenable() { return m_listenable; }

private:
    std::vector<std::optional<TileInst>> m_tiles;
    Listenable<Listener> m_listenable;
};

/// A top-level Project.
class Project
{
public:
    class Listener
    {
    public:
        virtual void add_layer(Layer&, int index) = 0;
        virtual void remove_layer(int index) = 0;
    };

    Project() = default;
    Project(Project&&) = default;
    Project& operator=(Project&&) = default;

    std::size_t tiledef_count() const;
    std::shared_ptr<TileDef> tiledef_at(int idx) const;

    void add_tiledef(TileDef tiledef);
    void remove_tiledef(int idx);

    std::size_t layer_count() const;
    const Layer& layer_at(int idx) const;
    Layer& layer_at(int idx);

    void add_layer();
    void add_layer(std::unique_ptr<Layer>, int idx);
    std::unique_ptr<Layer> remove_layer(int idx);

    Listenable<Listener>& add_layer_listenable()
    {
        return m_add_layer_listenable;
    }

private:
    std::vector<std::shared_ptr<TileDef>> m_tile_defs;
    std::vector<std::unique_ptr<Layer>> m_layers;
    Listenable<Listener> m_add_layer_listenable;
};

#endif
