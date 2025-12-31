#ifndef REDUCER_H
#define REDUCER_H

#include "Listener.h"

#include "noworry/Gfx3D/Model.h"
#include "noworry/Material/TextureMaterial.h"

#include <glm/ext/vector_int2.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class TileRotations;
class Project;
class World;
class Level;
class Layer;

enum class Rotation
{
    Rotate0,
    Rotate90,
    Rotate180,
    Rotate270
};

struct LayerLocation
{
    LayerLocation();

    int world;
    glm::ivec2 level;
    int layer;
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
    TileDef();

    const std::optional<std::shared_ptr<ModelData>>& model_data() const;
    const std::optional<std::shared_ptr<Model>>& model() const;

    short width() const;
    short depth() const;

    void set_model_data(std::optional<std::shared_ptr<ModelData>>);
    void set_model(std::optional<std::shared_ptr<Model>>);

    void set_width(short width);
    void set_depth(short depth);

private:
    std::optional<std::shared_ptr<ModelData>> m_model_data;
    std::optional<std::shared_ptr<Model>> m_model;

    short m_width;
    short m_depth;
};

/// A TileInst instances a tile definition on the map.
struct TileInst
{
public:
    TileInst(std::shared_ptr<TileDef>, int, Rotation);

    std::shared_ptr<TileDef> def() const { return m_def; }
    int z() const { return m_z; }
    Rotation rotation() const { return m_rotation; }

private:
    std::shared_ptr<TileDef> m_def;
    short m_z;
    Rotation m_rotation;
};

bool operator==(const TileInst&, const TileInst&);

/// A Layer is a square grid of TileInsts.
struct Layer
{
public:
    class Listener
    {
    public:
        virtual void layer_changed() = 0;
    };

    Layer(Level&);
    Layer(Layer&&) noexcept = delete;
    Layer& operator=(Layer&&) noexcept = delete;

    const std::optional<TileInst>& at(int x, int y) const;

    void set(int x, int y, std::optional<TileInst> option);

    Level& level() { return *m_level; }
    const Level& level() const { return *m_level; }

    Listenable<Listener>& listenable() const
    {
        return m_listenable;
    }

private:
    Level* m_level;
    std::vector<std::optional<TileInst>> m_tiles;
    mutable Listenable<Listener> m_listenable;
};

class Level
{
public:
    class Listener
    {
    public:
        virtual void layer_added(int idx) = 0;
        virtual void layer_removed(Layer&, int idx) = 0;
    };

    Level(World&);
    Level(Level&&) = delete;
    Level& operator=(Level&&) = delete;

    std::size_t layer_count() const;
    const Layer& layer_at(int idx) const;
    Layer& layer_at(int idx);

    void add_layer();
    void add_layer(std::unique_ptr<Layer>, int idx);
    std::unique_ptr<Layer> remove_layer(int idx);

    World& world() { return *m_world; }
    const World& world() const { return *m_world; }
    Listenable<Listener>& listenable() const
    {
        return m_listenable;
    }

private:
    World* m_world;
    std::vector<std::unique_ptr<Layer>> m_layers;
    mutable Listenable<Listener> m_listenable;
};

class World
{
    using LevelTable = std::unordered_map<glm::ivec2, std::unique_ptr<Level>>;
public:
    class Listener
    {
    public:
        virtual void level_added(Level&, int x, int y) = 0;
        virtual void level_removed(Level&, int x, int y) = 0;
    };

    World(Project& project);
    World(World&&) = delete;
    World& operator=(World&&) = delete;

    LevelTable::iterator begin();
    LevelTable::const_iterator begin() const;

    LevelTable::iterator end();
    LevelTable::const_iterator end() const;

    Level& level_at(int x, int y);
    const Level& level_at(int x, int y) const;

    Project& project() { return *m_project; }
    const Project& project() const { return *m_project; }

    Listenable<Listener>& listenable() const
    {
        return m_listenable;
    }

private:
    Project* m_project;
    LevelTable m_levels;
    mutable Listenable<Listener> m_listenable;
};

/// A top-level Project.
class Project
{
public:
    class Listener
    {
    public:
        virtual void world_added(World&, int idx) = 0;
        virtual void world_removed(World&, int idx) = 0;
    };

    Project();
    Project(Project&&) = delete;
    Project& operator=(Project&&) = delete;

    std::size_t tiledef_count() const;
    std::shared_ptr<TileDef> tiledef_at(int idx) const;
    void set_tiledef(int idx, TileDef tiledef);

    void add_tiledef(TileDef tiledef);
    void remove_tiledef(int idx);

    std::size_t world_count() const;
    World& world_at(int idx);
    const World& world_at(int idx) const;
    void add_world(std::unique_ptr<World>, int idx);
    std::unique_ptr<World> remove_world(int idx);

    Level& level_at(const LayerLocation&);
    const Level& level_at(const LayerLocation&) const;

    Layer& layer_at(const LayerLocation&);
    const Layer& layer_at(const LayerLocation&) const;

    Listenable<Listener>& listenable() const
    {
        return m_listenable;
    }

private:
    std::vector<std::shared_ptr<TileDef>> m_tile_defs;
    std::vector<std::unique_ptr<World>> m_worlds;
    mutable Listenable<Listener> m_listenable;
};

#endif
