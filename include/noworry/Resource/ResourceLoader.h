#ifndef RESOURCE_LOADER_H
#define RESOURCE_LOADER_H

#include "../resourcetable.h"

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

    std::unique_ptr<Mesh> load_mesh(
        ResourceTable& resources,
        aiMesh* ai_mesh);

    bool load_node(
        Model& model,
        const aiScene* ai_scene,
        const glm::mat4& parent_transform,
        aiNode* ai_node);
};

#endif
