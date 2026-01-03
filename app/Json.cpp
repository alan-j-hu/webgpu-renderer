#include "Json.h"
#include "noworry/Resource/ResourceLoader.h"

ImportError::ImportError(std::string message)
    : m_message(std::move(message))
{
}

JsonImporter::JsonImporter(ResourceTable& resources)
    : m_resources(&resources)
{
}

Project JsonImporter::load_project(const nlohmann::json& json)
{
    if (!json.is_object()) {
        throw ImportError("Expected object");
    }

    const nlohmann::json& tilesets_json = json.at("tilesets");
    if (!tilesets_json.is_array()) {
        throw ImportError("Expected array of tilesets");
    }

    m_tileset_buf.clear();
    for (auto& tileset_json : tilesets_json) {
        std::unique_ptr<Tileset> tileset = load_tileset(json);
        m_tileset_buf.push_back(
            std::make_shared<Tileset>(std::move(*tileset))
        );
    }

    const nlohmann::json& worlds_json = json.at("worlds");
    if (!worlds_json.is_array()) {
        throw ImportError("Expected array of worlds");
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

std::unique_ptr<Tileset> JsonImporter::load_tileset(const nlohmann::json& json)
{
    std::unique_ptr<Tileset> tileset;

    const nlohmann::json& tiles = json.at("tiles");
    if (!tiles.is_array()) {
        throw ImportError("Expected array of tile definitions");
    }

    for (auto& elt : tiles) {
        auto tile_def = load_tiledef(elt);
        tileset->add(*tile_def);
    }

    return tileset;
}

std::unique_ptr<TileDef> JsonImporter::load_tiledef(const nlohmann::json& json)
{
    short width = 0;
    short depth = 0;
    json.at("width").get_to(width);
    json.at("depth").get_to(depth);

    std::filesystem::path model_path;
    json.at("path").get_to(model_path);

    auto model_opt = m_resources->load<Model>(model_path);
    auto model_data_opt = m_resources->load<ModelData>(model_path);
    if (!model_opt || !model_data_opt) {
        throw ImportError("Could not load model: " + model_path.string());
    }

    std::unique_ptr<TileDef> tiledef;
    tiledef->set_width(width);
    tiledef->set_depth(depth);
    tiledef->set_model_path(std::move(model_path));
    tiledef->set_model_data(*model_data_opt);
    tiledef->set_model(*model_opt);

    return tiledef;
}

World JsonImporter::load_world(
    std::span<const std::shared_ptr<Tileset>> tilesets,
    const nlohmann::json& json)
{
    if (!json.is_object()) {
        throw ImportError("Expected JSON object");
    }

    int tileset_idx;
    json.at("tileset").get_to(tileset_idx);
    if (tileset_idx < 0 || tileset_idx >= tilesets.size()) {
        throw ImportError("Tileset index out of range");
    }

    const nlohmann::json& levels_json = json.at("levels");
    if (!levels_json.is_array()) {
        throw ImportError("Expected levels to be array");
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

Level JsonImporter::load_level(
    const Tileset& tileset,
    int level_width,
    int level_depth,
    const nlohmann::json& json)
{
    if (!json.is_object()) {
        throw ImportError("Expected JSON object");
    }

    const nlohmann::json& layers_json = json.at("layers");
    if (!layers_json.is_array()) {
        throw ImportError("Expected layers to be array");
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

Layer JsonImporter::load_layer(
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
        throw ImportError("Unknown format: " + format);
    }
}

Layer JsonImporter::load_layer_dense(
    const Tileset& tileset,
    int layer_width,
    int layer_depth,
    const nlohmann::json& json)
{
    Layer layer;

    const nlohmann::json& rows = json.at("tiles");
    if (!rows.is_array()) {
        throw ImportError("Expected JSON array");
    }
    if (rows.size() != layer_depth) {
        throw ImportError("Layer is wrong depth");
    }

    int y = 0;
    for (auto& row : rows) {
        if (!row.is_array()) {
            throw ImportError("Expected JSON array");
        }
        if (row.size() != layer_width) {
            throw ImportError("Layer is wrong width");
        }

        int x = 0;
        for (auto& elt : row) {
            if (elt.is_null()) {
                ++x;
                continue;
            }
            if (!elt.is_object()) {
                throw ImportError("Expected JSON tile instance object");
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

Layer JsonImporter::load_layer_sparse(
    const Tileset& tileset,
    int layer_width,
    int layer_depth,
    const nlohmann::json& json)
{
    Layer layer;

    const nlohmann::json& tiles = json.at("tiles");
    if (!tiles.is_array()) {
        throw ImportError("Expected JSON array");
    }

    for (auto& elt : tiles) {
        if (!elt.is_object()) {
            throw ImportError("Expected JSON tile instance object");
        }

        int x;
        int y;
        elt.at("x").get_to(x);
        elt.at("y").get_to(y);
        if (x < 0 || x >= layer_width || y < 0 || y >= layer_depth) {
            throw ImportError("Coordinates out of range ");
        }

        short z;
        elt.at("z").get_to(z);

        int tile_idx;
        elt.at("tile").get_to(tile_idx);
        if (tile_idx < 0 || tile_idx >= tileset.count()) {
            throw ImportError("Tile index out of range");
        }

        auto rotation = load_rotation(elt.at("rotation"));
        layer.set(x, y, TileInst(tileset.at(tile_idx), z, rotation));
    }

    return layer;
}

Rotation JsonImporter::load_rotation(const nlohmann::json& json)
{
    if (!json.is_string()) {
        throw ImportError("Expected string for rotation");
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

    throw ImportError("Got unknown rotation " + str);
}
