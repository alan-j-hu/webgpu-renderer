#ifndef REDUCER_H
#define REDUCER_H

#include "Listener.h"
#include "NameMap.h"

#include "noworry/Gfx3D/DynamicModel.h"
#include "noworry/Gfx3D/Model.h"
#include "noworry/Material/TextureMaterial.h"

#include <glm/ext/vector_int2.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include <filesystem>
#include <map>
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

class TileDef
{
public:
    class Listener
    {
    public:
        virtual void changed();
    };

    TileDef();

    const std::filesystem::path& model_path() const;
    const std::optional<std::shared_ptr<ModelData>>& model_data() const;
    const std::optional<std::shared_ptr<Model>>& model() const;

    short width() const;
    short depth() const;

    void set_model_path(std::filesystem::path);
    void set_model_data(std::optional<std::shared_ptr<ModelData>>);
    void set_model(std::optional<std::shared_ptr<Model>>);

    void set_width(short width);
    void set_depth(short depth);

private:
    std::filesystem::path m_model_path;
    std::optional<std::shared_ptr<ModelData>> m_model_data;
    std::optional<std::shared_ptr<Model>> m_model;

    short m_width;
    short m_depth;
};

class Tileset
{
public:
    class Listener
    {
    public:
        virtual void tile_added(int idx) = 0;
        virtual void tile_removed(TileDef&, int idx) = 0;
        virtual void tile_replaced(int idx) = 0;
    };

    std::size_t count() const;
    std::shared_ptr<TileDef> at(int idx);
    std::shared_ptr<const TileDef> at(int idx) const;
    void set(int idx, TileDef tiledef);

    void add(TileDef tiledef);
    std::shared_ptr<TileDef> remove(int idx);

    Listenable<Listener>& listenable() const { return m_listenable; }

private:
    std::vector<std::shared_ptr<TileDef>> m_tiles;
    mutable Listenable<Listener> m_listenable;
};

/// A TileInst instances a tile definition on the map.
struct TileInst
{
public:
    TileInst(std::shared_ptr<const TileDef>, int, Rotation);

    std::shared_ptr<const TileDef> def() const { return m_def; }
    int z() const { return m_z; }
    Rotation rotation() const { return m_rotation; }

private:
    std::shared_ptr<const TileDef> m_def;
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

    Layer();
    Layer(Layer&&) = default;
    Layer& operator=(Layer&&) = default;

    const std::optional<TileInst>& at(int x, int y) const;

    void set(int x, int y, std::optional<TileInst> option);

    void fill_model(DynamicModel&) const;

    Listenable<Listener>& listenable() const
    {
        return m_listenable;
    }

private:
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

    Level();

    template<class It>
    Level(It it, It end)
    {
        for (; it != end; ++it) {
            m_layers.push_back(std::make_unique<Layer>(std::move(*it)));
        }
        if (m_layers.size() == 0) {
            m_layers.push_back(std::make_unique<Layer>());
        }
    }

    Level(Level&&) = default;

    Level& operator=(Level&&) = default;

    int x() const { return m_x; }
    int y() const { return m_y; }

    void set_loc(int x, int y)
    {
        m_x = x;
        m_y = y;
    }

    std::size_t layer_count() const;
    const Layer& layer_at(int idx) const;
    Layer& layer_at(int idx);

    void add_layer();
    void add_layer(std::unique_ptr<Layer>, int idx);
    std::unique_ptr<Layer> remove_layer(int idx);

    Listenable<Listener>& listenable() const
    {
        return m_listenable;
    }

private:
    std::vector<std::unique_ptr<Layer>> m_layers;
    int m_x = 0;
    int m_y = 0;
    mutable Listenable<Listener> m_listenable;
};

class World
{
public:
    using iterator = NameMap<std::unique_ptr<Level>>::iterator;

    class Listener
    {
    public:
        virtual void level_added(Level&, int x, int y) = 0;
        virtual void level_removed(Level&, int x, int y) = 0;
    };

    struct InsertionInfo
    {
        int x;
        int y;
        std::string name;
        std::unique_ptr<Level> level;
    };

    World(std::shared_ptr<const Tileset>);

    template<class It>
    World(std::shared_ptr<const Tileset> tileset,
          int width, int depth,
          It it, It end)
        : m_tileset(std::move(tileset)),
          m_grid_width(width),
          m_grid_depth(depth)
    {
        for (; it != end; ++it) {
            insert_level(std::move(*it));
        }

        if (m_levels_by_name.size() == 0) {
            InsertionInfo info = {
                0,
                0,
                "default",
                std::make_unique<Level>()
            };
            insert_level(std::move(info));
        }
    }

    World(World&&) = default;

    World& operator=(World&&) = default;

    std::shared_ptr<const Tileset> tileset() const;

    iterator begin() const;
    iterator end() const;

    int grid_width() const;
    int grid_depth() const;

    Level* level_at(const std::string&);
    const Level* level_at(const std::string&) const;

    Level& level_at(int x, int y);
    const Level& level_at(int x, int y) const;

    bool insert_level(InsertionInfo);

    Listenable<Listener>& listenable() const
    {
        return m_listenable;
    }

private:
    NameMap<std::unique_ptr<Level>> m_levels_by_name;
    std::unordered_map<glm::ivec2, Level*> m_levels_by_coord;
    std::shared_ptr<const Tileset> m_tileset;
    int m_grid_width;
    int m_grid_depth;
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

    template<class TilesetIt, class WorldIt>
    Project(TilesetIt it1, TilesetIt end1, WorldIt it2, WorldIt end2) {
        for (; it1 != end1; ++it1) {
            m_tilesets.push_back(std::move(*it1));
        }
        if (m_tilesets.size() == 0) {
            m_tilesets.push_back(std::make_shared<Tileset>());
        }

        for (; it2 != end2; ++it2) {
            m_worlds.push_back(std::move(*it2));
        }
        if (m_worlds.size() == 0) {
            m_worlds.push_back(std::make_unique<World>(m_tilesets.at(0)));
        }
    }

    Project(Project&&) = default;
    Project& operator=(Project&&) = default;

    std::size_t tileset_count() const;
    std::shared_ptr<Tileset> tileset_at(int idx);
    std::shared_ptr<const Tileset> tileset_at(int idx) const;
    void add_tileset(std::shared_ptr<Tileset>);

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
    std::vector<std::shared_ptr<Tileset>> m_tilesets;
    std::vector<std::unique_ptr<World>> m_worlds;
    mutable Listenable<Listener> m_listenable;
};

#endif
