#include "Project.h"
#include <algorithm>
#include <utility>

LayerLocation::LayerLocation()
    : world(0), level(0, 0), layer(0)
{
}

TileDef::TileDef()
{
    m_width = 1;
    m_depth = 1;
}

const std::optional<std::shared_ptr<ModelData>>& TileDef::model_data() const
{
    return m_model_data;
}

const std::optional<std::shared_ptr<Model>>& TileDef::model() const
{
    return m_model;
}

short TileDef::width() const
{
    return m_width;
}

short TileDef::depth() const
{
    return m_depth;
}

void TileDef::set_model_data(
    std::optional<std::shared_ptr<ModelData>> model_data)
{
    m_model_data = model_data;
}

void TileDef::set_model(std::optional<std::shared_ptr<Model>> model)
{
    m_model = model;
}

void TileDef::set_width(short width)
{
    m_width = std::clamp<short>(width, 1, 5);
}

void TileDef::set_depth(short depth)
{
    m_depth = std::clamp<short>(depth, 1, 5);
}

TileInst::TileInst(std::shared_ptr<TileDef> def, int z, Rotation rotation)
    : m_def(std::move(def)), m_z(z), m_rotation(rotation)
{
}

bool operator==(const TileInst& lhs, const TileInst& rhs)
{
    return lhs.z() == rhs.z() && lhs.def().get() == rhs.def().get();
}

Layer::Layer()
{
    for (int i = 0; i < 16 * 16; ++i) {
        m_tiles.push_back(std::nullopt);
    }
}

const std::optional<TileInst>& Layer::at(int x, int y) const
{
    return m_tiles.at(y * 16 + x);
}

void Layer::set(int x, int y, std::optional<TileInst> option)
{
    m_tiles[y * 16 + x] = std::move(option);
    m_listenable.notify(&Layer::Listener::notify, *this);
}

Level::Level()
{
    m_layers.push_back(std::make_unique<Layer>());
}

std::size_t Level::layer_count() const
{
    return m_layers.size();
}

const Layer& Level::layer_at(int idx) const
{
    return *m_layers.at(idx);
}

Layer& Level::layer_at(int idx)
{
    return *m_layers.at(idx);
}

void Level::add_layer()
{
    m_layers.push_back(std::make_unique<Layer>());
    m_listenable.notify(
        &Level::Listener::add_layer,
        *m_layers.at(m_layers.size() - 1),
        m_layers.size() - 1);
}

void Level::add_layer(std::unique_ptr<Layer> layer, int idx)
{
    if (idx < 0 || idx > m_layers.size()) {
        return;
    }
    m_layers.insert(m_layers.begin() + idx, std::move(layer));
    m_listenable.notify(
        &Level::Listener::add_layer,
        *m_layers.at(idx),
        idx);
}

std::unique_ptr<Layer> Level::remove_layer(int idx)
{
    if (idx < 0 || idx >= m_layers.size()) {
        return nullptr;
    }

    if (m_layers.size() < 2) {
        return nullptr;
    }

    std::unique_ptr<Layer> layer = std::move(m_layers.at(idx));
    m_layers.erase(m_layers.begin() + idx);
    m_listenable.notify(&Level::Listener::remove_layer, idx);
    return layer;
}

World::World()
{
    m_levels.insert(std::pair(glm::ivec2(0, 0), std::make_unique<Level>()));
}

Level& World::level_at(int x, int y)
{
    return *m_levels.at(glm::ivec2(x, y));
}

const Level& World::level_at(int x, int y) const
{
    return *m_levels.at(glm::ivec2(x, y));
}

Project::Project()
{
    m_worlds.emplace_back();
}

std::size_t Project::tiledef_count() const
{
    return m_tile_defs.size();
}

std::shared_ptr<TileDef> Project::tiledef_at(int idx) const
{
    return m_tile_defs.at(idx);
}

void Project::set_tiledef(int idx, TileDef tiledef)
{
    m_tile_defs.at(idx) = std::make_shared<TileDef>(std::move(tiledef));
}

void Project::add_tiledef(TileDef tiledef)
{
    m_tile_defs.push_back(std::make_shared<TileDef>(std::move(tiledef)));
}

void Project::remove_tiledef(int idx)
{
    if (idx < 0 || idx >= m_tile_defs.size()) {
        return;
    }
    m_tile_defs.erase(m_tile_defs.begin() + idx);
}

World& Project::world_at(int idx)
{
    return *m_worlds.at(idx);
}

const World& Project::world_at(int idx) const
{
    return *m_worlds.at(idx);
}

void Project::add_world(std::unique_ptr<World> world, int idx)
{
    if (idx < 0 || idx > m_worlds.size()) {
        return;
    }
    m_worlds.insert(m_worlds.begin() + idx, std::move(world));
}

std::unique_ptr<World> Project::remove_world(int idx)
{
    if (idx < 0 || idx >= m_worlds.size()) {
        return nullptr;
    }

    if (m_worlds.size() < 2) {
        return nullptr;
    }

    std::unique_ptr<World> world = std::move(m_worlds.at(idx));
    m_worlds.erase(m_worlds.begin() + idx);
    return world;
}
