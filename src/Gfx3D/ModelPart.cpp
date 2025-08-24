#include "noworry/Gfx3D/ModelPart.h"
#include <utility>

ModelPart::ModelPart(int mesh, int material, const glm::mat4& transform)
    : m_mesh(mesh), m_material(material), m_transform(transform)
{
}

int ModelPart::mesh() const
{
    return m_mesh;
}

int ModelPart::material() const
{
    return m_material;
}

const glm::mat4& ModelPart::transform() const
{
    return m_transform;
}

glm::mat4& ModelPart::transform()
{
    return m_transform;
}
