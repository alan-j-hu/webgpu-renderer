#include "Serialize.h"
#include <tuple>

nlohmann::json JsonSerializer::save_project(const Project& project)
{
    std::unordered_map<const TileDef*, int> tiledef_mapping;
    std::unordered_map<const Tileset*, int> tileset_mapping;

    std::vector<nlohmann::json> tileset_json;
    for (int i = 0; i < project.tileset_count(); ++i) {
        auto shared_ptr = project.tileset_at(i);
        tileset_json.push_back(save_tileset(tiledef_mapping, *shared_ptr));
        tileset_mapping[shared_ptr.get()] = i;
    }

    std::vector<nlohmann::json> world_json;
    for (int i = 0; i < project.world_count(); ++i) {
        auto& world = project.world_at(i);
        world_json.push_back(
            save_world(tileset_mapping, tiledef_mapping, world));
    }

    return {
        {"tilesets", tileset_json},
        {"worlds", world_json}
    };
}

nlohmann::json JsonSerializer::save_tileset(
    std::unordered_map<const TileDef*, int>& tiledef_mapping,
    const Tileset& tileset)
{
    std::vector<nlohmann::json> tiledef_json;

    for (int i = 0; i < tileset.count(); ++i) {
        auto shared_ptr = tileset.at(i);
        tiledef_json.push_back(save_tiledef(*shared_ptr));
        tiledef_mapping[shared_ptr.get()] = i;
    }

    return {
        {"tiles", tiledef_json}
    };
}

nlohmann::json JsonSerializer::save_tiledef(const TileDef& tiledef)
{
    return {
        {"depth", tiledef.depth()},
        {"model", tiledef.model_path()},
        {"width", tiledef.width()}
    };
}

nlohmann::json JsonSerializer::save_world(
    const std::unordered_map<const Tileset*, int>& tileset_mapping,
    const std::unordered_map<const TileDef*, int>& tiledef_mapping,
    const World& world)
{
    const std::unordered_map<const Level*, glm::ivec2> coord_mapping;

    std::vector<nlohmann::json> levels;
    for (auto it = world.begin(); it != world.end(); ++it) {
        levels.push_back(save_level(
            tiledef_mapping,
            it->first,
            *it->second));
    }

    return {
        {"levels", levels},
        {"tileset", tileset_mapping.at(world.tileset().get())}
    };
}

nlohmann::json JsonSerializer::save_level(
    const std::unordered_map<const TileDef*, int>& mapping,
    const std::string& name,
    const Level& level)
{
     std::vector<nlohmann::json> layer_json;
     for (int i = 0; i < level.layer_count(); ++i) {
         layer_json.push_back(save_layer(mapping, level.layer_at(i)));
     }

     return {
         {"name", name},
         {"layers", layer_json},
         {"x", level.x()},
         {"y", level.y()}
     };
}

nlohmann::json JsonSerializer::save_layer(
    const std::unordered_map<const TileDef*, int>& mapping,
    const Layer& layer)
{
    std::vector<std::tuple<int, int, const TileInst*>> tiles;

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            auto& opt = layer.at(x, y);
            if (opt) {
                tiles.push_back({x, y, &opt.value()});
            }
        }
    }

    if (true) {
        std::vector<nlohmann::json> tile_json;
        for (auto [x, y, inst] : tiles) {
            tile_json.push_back({
                {"rotation", save_rotation(inst->rotation())},
                {"tile", mapping.at(inst->def().get())},
                {"x", x},
                {"y", y},
                {"z", inst->z()}
            });
        }

        return {
            {"format", "sparse"},
            {"tiles", tile_json}
        };
    } else {
        std::vector<nlohmann::json> row_json;
        std::vector<nlohmann::json> tile_json;
        for (int y = 0; y < 16; ++y) {
            tile_json.clear();
            for (int x = 0; x < 16; ++x) {
                auto opt = layer.at(x, y);
                if (opt) {
                    tile_json.push_back({});
                } else {
                    tile_json.push_back({
                        {"rotation", save_rotation(opt.value().rotation())},
                        {"tile", mapping.at(opt.value().def().get())},
                        {"z", opt->z()}
                    });
                }
            }

            row_json.push_back(tile_json);
        }

        return {
            {"format", "dense"},
            {"tiles", row_json}
        };
    }
}

nlohmann::json JsonSerializer::save_rotation(Rotation rotation)
{
    switch (rotation) {
    case Rotation::Rotate0:
        return "0";
    case Rotation::Rotate90:
        return "90";
    case Rotation::Rotate180:
        return "180";
    case Rotation::Rotate270:
        return "270";
    }

    return "0";
}
