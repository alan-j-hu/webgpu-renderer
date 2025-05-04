#ifndef RESOURCE_TABLE_H
#define RESOURCE_TABLE_H

#include "mesh.h"
#include "texture.h"
#include "renderer.h"
#include "material/material.h"
#include "material/flatmaterial.h"
#include "material/texturematerial.h"
#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

class ResourceTable
{
public:
    ResourceTable(Renderer&);
    ResourceTable(const ResourceTable&) = delete;
    ResourceTable& operator=(const ResourceTable&) = delete;
    ResourceTable(ResourceTable&&) = default;
    ResourceTable& operator=(ResourceTable&&) = default;

    std::shared_ptr<Texture> add_texture(const std::filesystem::path& path);

    FlatMaterial& add_flat_material(float, float, float);

    TextureMaterial& add_texture_material(std::shared_ptr<Texture>);

    Mesh& add_mesh(
        Vertex* vertices,
        std::size_t vertex_count,
        std::uint16_t* indices,
        std::size_t index_count);

private:
    WGPUDevice m_device;
    Renderer* m_renderer;

    int m_mat_id;

    std::unordered_map<std::filesystem::path,
                       std::weak_ptr<Texture>> m_textures;
    std::vector<std::unique_ptr<Material>> m_materials;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
};

#endif
