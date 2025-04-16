#include "noworry/resourcetable.h"

ResourceTable::ResourceTable(Renderer& renderer)
    : m_renderer(renderer)
{
}

Texture& ResourceTable::add_texture(const std::filesystem::path& path)
{
    m_textures.push_back(std::make_unique<Texture>(m_renderer.device(), path));
    return *m_textures[m_textures.size() - 1];
}

FlatMaterial& ResourceTable::add_flat_material(float r, float g, float b)
{
    auto ptr = std::make_unique<FlatMaterial>(
        m_mat_id++,
        m_renderer.device(),
        m_renderer.flat_mesh_effect(),
        r, g, b);
    FlatMaterial& mat = *ptr;
    m_materials.emplace_back(std::move(ptr));
    return mat;
}

TextureMaterial& ResourceTable::add_texture_material(
    const Texture& texture)
{
    auto ptr = std::make_unique<TextureMaterial>(
        m_mat_id++,
        m_renderer.device(),
        m_renderer.texture_mesh_effect(),
        texture,
        m_renderer.default_sampler());
    TextureMaterial& mat = *ptr;
    m_materials.emplace_back(std::move(ptr));
    return mat;
}

Mesh& ResourceTable::add_mesh(
    Vertex* vertices,
    std::size_t vertex_count,
    std::uint16_t* indices,
    std::size_t tri_count)
{
    m_meshes.push_back(std::make_unique<Mesh>(
        m_renderer.device(), vertices, vertex_count, indices, tri_count));
    return *m_meshes[m_meshes.size() - 1];
}
