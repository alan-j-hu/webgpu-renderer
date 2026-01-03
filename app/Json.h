#ifndef JSON_IMPORT_EXPORT_H
#define JSON_IMPORT_EXPORT_H

#include "Project.h"
#include "noworry/resourcetable.h"
#include <nlohmann/json.hpp>

class ImportError
{
public:
    ImportError(std::string);

private:
    std::string m_message;
};

class JsonImporter
{
public:
    JsonImporter(ResourceTable&);

    Project load_project(const nlohmann::json&);

private:
    ResourceTable* m_resources;
    std::vector<std::shared_ptr<Tileset>> m_tileset_buf;
    std::vector<std::unique_ptr<World>> m_world_buf;
    std::vector<std::pair<glm::ivec2, Level>> m_level_buf;

    std::unique_ptr<TileDef> load_tiledef(const nlohmann::json&);

    std::unique_ptr<Tileset> load_tileset(const nlohmann::json&);

    World load_world(
        std::span<const std::shared_ptr<Tileset>>,
        const nlohmann::json&);

    Level load_level(
        const Tileset&,
        int level_width,
        int level_depth,
        const nlohmann::json&);

    Layer load_layer(
        const Tileset&,
        int layer_width,
        int layer_depth,
        const nlohmann::json&);

    Layer load_layer_dense(
        const Tileset&,
        int layer_width,
        int layer_depth,
        const nlohmann::json&);

    Layer load_layer_sparse(
        const Tileset&,
        int layer_width,
        int layer_depth,
        const nlohmann::json&);

    Rotation load_rotation(const nlohmann::json&);
};

#endif
