#include "noworry/Gfx3D/Model.h"
#include <utility>

std::span<const std::unique_ptr<Mesh>> Model::meshes() const
{
    return m_meshes;
}

std::span<const std::shared_ptr<Material>> Model::materials() const
{
    return m_materials;
}

std::span<const ModelPart> Model::parts() const
{
    return m_parts;
}

void Model::add_mesh(std::unique_ptr<Mesh> mesh)
{
    m_meshes.push_back(std::move(mesh));
}

void Model::add_material(std::shared_ptr<Material> material)
{
    m_materials.push_back(std::move(material));
}

void Model::add_part(ModelPart part)
{
    m_parts.push_back(std::move(part));
}
