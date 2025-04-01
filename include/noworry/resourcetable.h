#ifndef RESOURCE_TABLE_H
#define RESOURCE_TABLE_H

#include "material.h"
#include "mesh.h"
#include "model.h"
#include "texture.h"
#include "renderer.h"
#include <memory>
#include <vector>

class ResourceTable
{
public:
    ResourceTable(Renderer&);

    Texture& add_texture(const std::filesystem::path& path);

    Material& add_material(const Texture&);

    Mesh& add_mesh(
        Vertex* vertices,
        std::size_t vertex_count,
        std::uint16_t* indices,
        std::size_t tri_count);

    Model& add_model(const Mesh& mesh, Material& material);
private:

    WGPUDevice m_device;
    Renderer& m_renderer;

    int m_mat_id;

    std::vector<std::unique_ptr<Texture>> m_textures;
    std::vector<std::unique_ptr<Material>> m_materials;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::unique_ptr<Model>> m_models;
};

#endif
