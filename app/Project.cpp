#include "Project.h"
#include "glm/ext/matrix_transform.hpp"
#include <algorithm>
#include <utility>

glm::mat4 transform(
    float x, float y, float z,
    float width, float depth, Rotation rotation)
{
    static constexpr glm::vec3 XY_PLANE = glm::vec3(0, 0, 1);

    switch (rotation) {
    case Rotation::Rotate0:
        return glm::translate(glm::mat4(1), glm::vec3(x, y, z));
    case Rotation::Rotate90: {
        glm::mat4 translation = glm::translate(
            glm::mat4(1),
            glm::vec3(x + depth, y, z));
        return glm::rotate(
            translation,
            0.5f * std::numbers::pi_v<float>,
            XY_PLANE);
    }
    case Rotation::Rotate180: {
        glm::mat4 translation = glm::translate(
            glm::mat4(1),
            glm::vec3(x + width, y + depth, z));
        return glm::rotate(
            translation,
            std::numbers::pi_v<float>,
            XY_PLANE);
    }
    case Rotation::Rotate270: {
        glm::mat4 translation = glm::translate(
            glm::mat4(1),
            glm::vec3(x, y + width, z));
        return glm::rotate(
            translation,
            1.5f * std::numbers::pi_v<float>,
            XY_PLANE);
    }
    default:
        return glm::mat4(1);
    }
}

LayerLocation::LayerLocation()
    : world(0), level(0, 0), layer(0)
{
}

TileDef::TileDef()
{
    m_width = 1;
    m_depth = 1;
}

const std::filesystem::path& TileDef::model_path() const
{
    return m_model_path;
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

void TileDef::set_model_path(std::filesystem::path path)
{
    m_model_path = std::move(path);
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

std::size_t Tileset::count() const
{
    return m_tiles.size();
}

std::shared_ptr<TileDef> Tileset::at(int idx)
{
    return m_tiles.at(idx);
}

std::shared_ptr<const TileDef> Tileset::at(int idx) const
{
    return m_tiles.at(idx);
}

void Tileset::set(int idx, TileDef tiledef)
{
    m_tiles.at(idx) = std::make_shared<TileDef>(std::move(tiledef));
    m_listenable.notify(&Tileset::Listener::tile_replaced, idx);
}

void Tileset::add(TileDef tiledef)
{
    m_tiles.push_back(std::make_shared<TileDef>(std::move(tiledef)));
    m_listenable.notify(&Tileset::Listener::tile_added, m_tiles.size() - 1);
}

std::shared_ptr<TileDef> Tileset::remove(int idx)
{
    if (idx < 0 || idx >= m_tiles.size()) {
        return nullptr;
    }
    auto ptr = m_tiles.at(idx);
    m_tiles.erase(m_tiles.begin() + idx);

    m_listenable.notify(&Tileset::Listener::tile_removed, *ptr, idx);

    return ptr;
}

TileInst::TileInst(
    std::shared_ptr<const TileDef> def,
    int z,
    Rotation rotation)
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
    m_listenable.notify(&Layer::Listener::layer_changed);
}

void Layer::fill_model(DynamicModel& model) const
{
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            auto& opt = at(x, y);
            if (!opt) {
                continue;
            }

            auto& inst = opt.value();
            auto def = inst.def();
            if (!def->model_data()) {
                continue;
            }

            glm::mat4 matrix = transform(
                x, y, inst.z(),
                def->width(), def->depth(),
                inst.rotation());

            model.add_model(**inst.def()->model_data(), matrix);
        }
    }
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
    int idx = m_layers.size() - 1;
    m_listenable.notify(&Level::Listener::layer_added, idx);
}

void Level::add_layer(std::unique_ptr<Layer> layer, int idx)
{
    if (idx < 0 || idx > m_layers.size()) {
        return;
    }
    m_layers.insert(m_layers.begin() + idx, std::move(layer));
    m_listenable.notify(&Level::Listener::layer_added, idx);
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
    m_listenable.notify(&Level::Listener::layer_removed, *layer, idx);
    return layer;
}

World::World(std::shared_ptr<const Tileset> tileset)
    : m_tileset(std::move(tileset)), m_grid_width(1), m_grid_depth(1)
{
    InsertionInfo info = {
        0,
        0,
        "default",
        std::make_unique<Level>()
    };
    insert_level(std::move(info));
}

std::shared_ptr<const Tileset> World::tileset() const
{
    return m_tileset;
}

World::iterator World::begin() const
{
    return m_levels_by_name.begin();
}

World::iterator World::end() const
{
    return m_levels_by_name.end();
}

int World::grid_width() const
{
    return m_grid_width;
}

int World::grid_depth() const
{
    return m_grid_depth;
}

Level* World::level_at(const std::string& name)
{
    auto ptr = m_levels_by_name.find(name);
    if (ptr == nullptr) {
        return nullptr;
    }

    return ptr->get();
}

const Level* World::level_at(const std::string& name) const
{
    auto ptr = m_levels_by_name.find(name);
    if (ptr == nullptr) {
        return nullptr;
    }

    return ptr->get();
}

Level& World::level_at(int x, int y)
{
    return *m_levels_by_coord.at(glm::ivec2(x, y));
}

const Level& World::level_at(int x, int y) const
{
    return *m_levels_by_coord.at(glm::ivec2(x, y));
}

bool World::insert_level(World::InsertionInfo info)
{
    auto ivec2 = glm::ivec2(info.x, info.y);
    if (m_levels_by_coord.find(ivec2) != m_levels_by_coord.end()) {
        return false;
    }
    auto [k, v] = m_levels_by_name.insert(info.name, std::move(info.level));
    m_levels_by_coord.emplace(ivec2, v.get());
    v->set_loc(info.x, info.y);
    return true;
}

Project::Project()
{
    m_tilesets.push_back(std::make_shared<Tileset>());
    m_worlds.push_back(std::make_unique<World>(m_tilesets[0]));
}

std::size_t Project::tileset_count() const
{
    return m_tilesets.size();
}

std::shared_ptr<Tileset> Project::tileset_at(int idx)
{
    return m_tilesets.at(idx);
}

std::shared_ptr<const Tileset> Project::tileset_at(int idx) const
{
    return m_tilesets.at(idx);
}

void Project::add_tileset(std::shared_ptr<Tileset> tileset)
{
    m_tilesets.push_back(std::move(tileset));
}

std::size_t Project::world_count() const
{
    return m_worlds.size();
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

Level& Project::level_at(const LayerLocation& location)
{
    return world_at(location.world)
        .level_at(location.level.x, location.level.y);
}

const Level& Project::level_at(const LayerLocation& location) const
{
    return world_at(location.world)
        .level_at(location.level.x, location.level.y);
}

Layer& Project::layer_at(const LayerLocation& location)
{
    return world_at(location.world)
        .level_at(location.level.x, location.level.y)
        .layer_at(location.layer);
}

const Layer& Project::layer_at(const LayerLocation& location) const
{
    return world_at(location.world)
        .level_at(location.level.x, location.level.y)
        .layer_at(location.layer);
}
