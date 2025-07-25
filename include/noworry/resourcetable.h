#ifndef RESOURCE_TABLE_H
#define RESOURCE_TABLE_H

#include "mesh.h"
#include "texture.h"
#include "renderer.h"
#include "Material/Material.h"
#include "Material/FlatMaterial.h"
#include "Material/TextureMaterial.h"
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

/// A cache for loading and storing textures and materials. If a file has
/// already been loaded and is in use, it will not be loaded again.
///
/// This class keeps weak_ptrs to its resources, so it will unload resources
/// which are no longer in use.
class ResourceTable
{
public:
    ResourceTable(Renderer&);
    ResourceTable(const ResourceTable&) = delete;
    ResourceTable& operator=(const ResourceTable&) = delete;
    ResourceTable(ResourceTable&&) = default;
    ResourceTable& operator=(ResourceTable&&) = default;

    std::optional<std::shared_ptr<Texture>>
    load_texture(const std::filesystem::path& path);

    std::optional<std::shared_ptr<TextureMaterial>>
    load_texture_material(const std::filesystem::path& path);

    FlatMaterial& add_flat_material(float, float, float);

    FlatMaterial& add_wireframe_material(float, float, float);

    TextureMaterial& add_texture_material(std::shared_ptr<Texture>);

private:
    WGPUDevice m_device;
    Renderer* m_renderer;

    int m_mat_id;

    std::unordered_map<std::filesystem::path,
                       std::weak_ptr<Texture>> m_textures;
    std::unordered_map<std::filesystem::path,
                       std::weak_ptr<TextureMaterial>> m_texture_materials;
    std::vector<std::unique_ptr<Material>> m_materials;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
};

#endif
