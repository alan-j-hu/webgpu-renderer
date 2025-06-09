#include "noworry/resourcetable.h"
#include "noworry/material/wireframemesheffect.h"

ResourceTable::ResourceTable(Renderer& renderer)
    : m_renderer(&renderer)
{
}

std::optional<std::shared_ptr<Texture>>
ResourceTable::load_texture(const std::filesystem::path& path)
{
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        if (auto shared = it->second.lock()) {
            return shared;
        }
    }

    auto optional = Texture::from_path(m_renderer->device(), path);
    if (!optional) {
        return std::nullopt;
    }

    auto ptr = std::make_shared<Texture>(std::move(optional.value()));
    std::weak_ptr<Texture> handle = ptr;
    m_textures.emplace(path, std::move(handle));

    return std::optional(std::move(ptr));
}

std::optional<std::shared_ptr<TextureMaterial>>
ResourceTable::load_texture_material(const std::filesystem::path& path)
{
    auto it = m_texture_materials.find(path);
    if (it != m_texture_materials.end()) {
        if (auto shared = it->second.lock()) {
            return shared;
        }
    }

    auto optional = load_texture(path);
    if (!optional) {
        return std::nullopt;
    }

    auto material = std::make_shared<TextureMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->mesh_effect<TextureMeshEffect>(),
        std::move(optional.value()),
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
