#include "noworry/resourcetable.h"
#include "noworry/material/wireframemesheffect.h"

ResourceTable::ResourceTable(Renderer& renderer)
    : m_renderer(&renderer)
{
}

std::shared_ptr<Texture>
ResourceTable::load_texture(const std::filesystem::path& path)
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

std::shared_ptr<TextureMaterial>
ResourceTable::load_texture_material(const std::filesystem::path& path)
{
    auto it = m_texture_materials.find(path);
    if (it != m_texture_materials.end()) {
        if (auto shared = it->second.lock()) {
            return shared;
        }
    }

    auto texture = load_texture(path);
    auto material = std::make_shared<TextureMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->mesh_effect<TextureMeshEffect>(),
        std::move(texture),
        m_renderer->default_sampler());
    std::weak_ptr<TextureMaterial> handle = material;
    m_texture_materials.emplace(path, std::move(handle));
    return material;
}

FlatMaterial& ResourceTable::add_flat_material(float r, float g, float b)
{
    auto ptr = std::make_unique<FlatMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->mesh_effect<FlatMeshEffect>(),
        r, g, b);
    FlatMaterial& mat = *ptr;
    m_materials.emplace_back(std::move(ptr));
    return mat;
}

FlatMaterial& ResourceTable::add_wireframe_material(float r, float g, float b)
{
    auto ptr = std::make_unique<FlatMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->mesh_effect<WireframeMeshEffect>(),
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
        *m_renderer->mesh_effect<TextureMeshEffect>(),
        std::move(texture),
        m_renderer->default_sampler());
    TextureMaterial& mat = *ptr;
    m_materials.emplace_back(std::move(ptr));
    return mat;
}
