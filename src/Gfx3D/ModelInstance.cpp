#include "noworry/Gfx3D/ModelInstance.h"
#include "noworry/Gfx3D/Model.h"
#include "noworry/renderer.h"

ModelInstance::ModelInstance(const Model& model)
    : m_model(&model), m_transform(glm::mat4(1))
{
    for (auto& model_part : model.parts()) {
        m_parts.push_back(glm::mat4(1));
    }
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
    auto meshes = m_model->meshes();
    auto materials = m_model->materials();
    auto model_parts = m_model->parts();
    for (int i = 0; i < model_parts.size(); ++i) {
        m_parts[i] = model_parts[i].transform() * m_transform;
    }

    for (int i = 0; i < model_parts.size(); ++i) {
        const Mesh& mesh = *meshes[model_parts[i].mesh()];
        const Material& material = *materials[model_parts[i].material()];
        frame.add(mesh, material, m_parts[i]);
    }
}
