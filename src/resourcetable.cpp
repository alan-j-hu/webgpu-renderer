#include "noworry/resourcetable.h"
#include "noworry/Gfx3D/Model.h"
#include "noworry/Material/Effect.h"

ResourceTable::ResourceTable(Renderer& renderer)
    : m_renderer(&renderer)
{
    m_default_material = std::make_shared<FlatMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->effect<FlatEffect>(),
        0.5, 0.5, 0.5);
}

FlatMaterial& ResourceTable::add_flat_material(float r, float g, float b)
{
    auto ptr = std::make_unique<FlatMaterial>(
        m_mat_id++,
        m_renderer->device(),
        *m_renderer->effect<FlatEffect>(),
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
        *m_renderer->effect<FlatEffect>(),
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
        *m_renderer->effect<TextureEffect>(),
        std::move(texture),
        m_renderer->default_sampler());
    TextureMaterial& mat = *ptr;
    m_materials.emplace_back(std::move(ptr));
    return mat;
}
