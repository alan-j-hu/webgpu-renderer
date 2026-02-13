#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include "../resourcetable.h"
#include "../Gfx3D/ModelData.h"

#include <memory>
#include <optional>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

template<>
class ResourceLoader<Texture>
{
public:
    std::optional<std::shared_ptr<Texture>> load(
        ResourceTable& resources,
        const std::filesystem::path& path);
};

template<>
class ResourceLoader<TextureMaterial>
{
public:
    std::optional<std::shared_ptr<TextureMaterial>> load(
        ResourceTable& resources,
        const std::filesystem::path& path);
};

template<>
class ResourceLoader<Model>
{
public:
    std::optional<std::shared_ptr<Model>> load(
        ResourceTable& resources,
        const std::filesystem::path& path);
};

template<>
class ResourceLoader<ModelData>
{
public:
    std::optional<std::shared_ptr<ModelData>> load(
        ResourceTable& resources,
        const std::filesystem::path& path);

private:
    bool load_node(
        ResourceTable& resources,
        const std::filesystem::path& path,
        ModelData& model,
        const aiScene* ai_scene,
        aiNode* ai_node);
};

#endif
