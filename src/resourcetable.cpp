#include "noworry/resourcetable.h"

ResourceTable::ResourceTable(Renderer& renderer)
    : m_renderer(&renderer)
{
}

std::shared_ptr<Texture>
ResourceTable::add_texture(const std::filesystem::path& path)
{
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        if (auto shared = it->second.lock()) {
            return shared;
        }
    }

    auto texture = std::make_shared<Texture>(m_renderer->device(), path);
    std::weak_ptr<Texture> handle = texture;
    m_textures.emplace(path, std::move(handle));
    return texture;
}

FlatMaterial& ResourceTable::add_flat_material(float r, float g, float b)
{
    auto ptr = std::make_unique<FlatMaterial>(
        m_mat_id++,
        m_renderer->device(),
        m_renderer->flat_mesh_effect(),
        r, g, b);
    FlatMaterial& mat = *ptr;
    m_materials.emplace_back(std::move(ptr));
    return mat;
}

TextureMaterial& ResourceTable::add_texture_material(
    std::shared_ptr<Texture> texture)
{
    auto ptr = std::make_unique<TextureMaterial>(
        m_mat_id++,
        m_renderer->device(),
        m_renderer->texture_mesh_effect(),
        std::move(texture),
        m_renderer->default_sampler());
    TextureMaterial& mat = *ptr;
    m_materials.emplace_back(std::move(ptr));
    return mat;
}

Mesh& ResourceTable::add_mesh(
    Vertex* vertices,
    std::size_t vertex_count,
    std::uint16_t* indices,
    std::size_t index_count)
{
    m_meshes.push_back(std::make_unique<Mesh>(
        m_renderer->device(), vertices, vertex_count, indices, index_count));
    return *m_meshes[m_meshes.size() - 1];
}
