#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "Project.h"
#include <nlohmann/json.hpp>

class JsonSerializer
{
public:
    nlohmann::json save_project(const Project&);

private:
    nlohmann::json save_tileset(
        std::unordered_map<const TileDef*, int>&,
        const Tileset&);

    nlohmann::json save_tiledef(const TileDef&);

    nlohmann::json save_world(
        const std::unordered_map<const Tileset*, int>& tileset_mapping,
        const std::unordered_map<const TileDef*, int>& tiledef_mapping,
        const World& world);

    nlohmann::json save_level(
        const std::unordered_map<const TileDef*, int>&,
        const std::string& name,
        const Level&);

    nlohmann::json save_layer(
        const std::unordered_map<const TileDef*, int>&,
        const Layer&);

    nlohmann::json save_rotation(Rotation);
};

#endif
