#include "Deserialize.h"
#include "noworry/Resource/ResourceLoader.h"

DeserializeError::DeserializeError(std::string message)
    : m_message(std::move(message))
{
}

const char* DeserializeError::what() const noexcept
{
    return m_message.c_str();
}

JsonDeserializer::JsonDeserializer(ResourceTable& resources)
    : m_resources(&resources)
{
}

Project JsonDeserializer::load_project(
    const std::filesystem::path& working_dir,
    const nlohmann::json& json)
{
    if (!json.is_object()) {
        throw DeserializeError("Expected object at top level");
    }

    const nlohmann::json& tilesets_json = json.at("tilesets");
    if (!tilesets_json.is_array()) {
        throw DeserializeError("Expected array of tilesets");
    }

    m_tileset_buf.clear();
    for (auto& tileset_json : tilesets_json) {
        std::unique_ptr<Tileset> tileset =
            load_tileset(working_dir, tileset_json);
        m_tileset_buf.push_back(
            std::make_shared<Tileset>(std::move(*tileset))
        );
    }

    const nlohmann::json& worlds_json = json.at("worlds");
    if (!worlds_json.is_array()) {
        throw DeserializeError("Expected array of worlds");
    }

    m_world_buf.clear();
    for (auto& world_json : worlds_json) {
        World world = load_world(std::span(m_tileset_buf), world_json);
        m_world_buf.push_back(std::make_unique<World>(std::move(world)));
    }

    return Project(
        m_tileset_buf.begin(),
        m_tileset_buf.end(),
        m_world_buf.begin(),
        m_world_buf.end()
    );
}

std::unique_ptr<Tileset>
JsonDeserializer::load_tileset(
    const std::filesystem::path& working_dir,
    const nlohmann::json& json)
{
    std::unique_ptr<Tileset> tileset = std::make_unique<Tileset>();

    const nlohmann::json& tiles = json.at("tiles");
    if (!tiles.is_array()) {
        throw DeserializeError("Expected array of tile definitions");
    }

    for (auto& elt : tiles) {
        auto tile_def = load_tiledef(working_dir, elt);
        tileset->add(*tile_def);
    }

    return tileset;
}

std::unique_ptr<TileDef>
JsonDeserializer::load_tiledef(
    const std::filesystem::path& working_dir,
    const nlohmann::json& json)
{
    short width = 0;
    short depth = 0;
    json.at("width").get_to(width);
    json.at("depth").get_to(depth);

    std::filesystem::path model_path;
    json.at("model").get_to(model_path);

    const std::filesystem::path full_path = working_dir / model_path;
    auto model_opt = m_resources->load<Model>(full_path);
    auto model_data_opt = m_resources->load<ModelData>(full_path);
    if (!model_opt || !model_data_opt) {
        throw DeserializeError("Could not load model: " + full_path.string());
    }

    std::unique_ptr<TileDef> tiledef = std::make_unique<TileDef>();
    tiledef->set_width(width);
    tiledef->set_depth(depth);
    tiledef->set_model_path(std::move(model_path));
    tiledef->set_model_data(*model_data_opt);
    tiledef->set_model(*model_opt);

    return tiledef;
}

World JsonDeserializer::load_world(
    std::span<const std::shared_ptr<Tileset>> tilesets,
    const nlohmann::json& json)
{
    if (!json.is_object()) {
        throw DeserializeError("Expected object for world");
    }

    int tileset_idx;
    json.at("tileset").get_to(tileset_idx);
    if (tileset_idx < 0 || tileset_idx >= tilesets.size()) {
        throw DeserializeError("Tileset index out of range");
    }

    const nlohmann::json& levels_json = json.at("levels");
    if (!levels_json.is_array()) {
        throw DeserializeError("Expected levels to be array");
    }

    m_level_buf.clear();
    for (auto& elt : levels_json) {
        int x;
        int y;
        elt.at("x").get_to(x);
        elt.at("y").get_to(y);

        Level level = load_level(*tilesets[tileset_idx], 16, 16, elt);
        m_level_buf.push_back(
            std::make_pair(glm::ivec2(x, y), std::move(level))
        );
    }

    return World(
        tilesets[tileset_idx],
        m_level_buf.begin(),
        m_level_buf.end()
    );
}

Level JsonDeserializer::load_level(
    const Tileset& tileset,
    int level_width,
    int level_depth,
    const nlohmann::json& json)
{
    if (!json.is_object()) {
        throw DeserializeError("Expected object for level");
    }

    const nlohmann::json& layers_json = json.at("layers");
    if (!layers_json.is_array()) {
        throw DeserializeError("Expected layers to be array");
    }

    std::vector<Layer> layers;
    for (auto& elt : layers_json) {
        Layer layer = load_layer(
            tileset,
            level_width,
            level_depth,
            elt
        );

        layers.push_back(std::move(layer));
    }

    return Level(layers.begin(), layers.end());
}

Layer JsonDeserializer::load_layer(
    const Tileset& tileset,
    int layer_width,
    int layer_depth,
    const nlohmann::json& json)
{
    std::string format;
    json.at("format").get_to(format);
    if (format == "dense") {
        return load_layer_dense(tileset, layer_width, layer_depth, json);
    } else if (format == "sparse") {
        return load_layer_sparse(tileset, layer_width, layer_depth, json);
    } else {
        throw DeserializeError("Unknown format: " + format);
    }
}

Layer JsonDeserializer::load_layer_dense(
    const Tileset& tileset,
    int layer_width,
    int layer_depth,
    const nlohmann::json& json)
{
    Layer layer;

    const nlohmann::json& rows = json.at("tiles");
    if (!rows.is_array()) {
        throw DeserializeError("Expected array");
    }
    if (rows.size() != layer_depth) {
        throw DeserializeError("Layer is wrong depth");
    }

    int y = 0;
    for (auto& row : rows) {
        if (!row.is_array()) {
            throw DeserializeError("Expected array");
        }
        if (row.size() != layer_width) {
            throw DeserializeError("Layer is wrong width");
        }

        int x = 0;
        for (auto& elt : row) {
            if (elt.is_null()) {
                ++x;
                continue;
            }
            if (!elt.is_object()) {
                throw DeserializeError("Expected object for tile instance");
            }

            short z;
            elt.at("z").get_to(z);

            int tile_idx;
            elt.at("tile").get_to(tile_idx);

            auto rotation = load_rotation(elt.at("rotation"));
            layer.set(x, y, TileInst(tileset.at(tile_idx), z, rotation));

            ++x;
        }

        ++y;
    }

    return layer;
}

Layer JsonDeserializer::load_layer_sparse(
    const Tileset& tileset,
    int layer_width,
    int layer_depth,
    const nlohmann::json& json)
{
    Layer layer;

    const nlohmann::json& tiles = json.at("tiles");
    if (!tiles.is_array()) {
        throw DeserializeError("Expected JSON array");
    }

    for (auto& elt : tiles) {
        if (!elt.is_object()) {
            throw DeserializeError("Expected object for tile instance");
        }

        int x = -1;
        int y = -1;
        elt.at("x").get_to(x);
        elt.at("y").get_to(y);
        if (x < 0 || x >= layer_width || y < 0 || y >= layer_depth) {
            throw DeserializeError("Coordinates out of range");
        }

        short z = -1;
        elt.at("z").get_to(z);

        int tile_idx;
        elt.at("tile").get_to(tile_idx);
        if (tile_idx < 0 || tile_idx >= tileset.count()) {
            throw DeserializeError("Tile index out of range");
        }

        auto rotation = load_rotation(elt.at("rotation"));
        layer.set(x, y, TileInst(tileset.at(tile_idx), z, rotation));
    }

    return layer;
}

Rotation JsonDeserializer::load_rotation(const nlohmann::json& json)
{
    if (!json.is_string()) {
        throw DeserializeError("Expected string for rotation");
    }
    std::string str;
    json.get_to(str);

    if (str == "0") {
        return Rotation::Rotate0;
    } else if (str == "90") {
        return Rotation::Rotate90;
    } else if (str == "180") {
        return Rotation::Rotate180;
    } else if (str == "270") {
        return Rotation::Rotate270;
    }

    throw DeserializeError("Got unknown rotation " + str);
}
