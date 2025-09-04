#include "noworry/Gfx3D/ModelInstance.h"
#include "noworry/Gfx3D/Model.h"
#include "noworry/renderer.h"
#include <iostream>

ModelInstance::ModelInstance(const Model& model)
    : m_model(&model), m_transform(glm::mat4(1))
{
}

const glm::mat4& ModelInstance::transform() const
{
    return m_transform;
}

glm::mat4& ModelInstance::transform()
{
    return m_transform;
}

void ModelInstance::render(Frame& frame)
{
    auto parts = m_model->parts();
    for (auto& part : parts) {
        frame.add(part.mesh(), part.material(), m_transform);
    }
}
