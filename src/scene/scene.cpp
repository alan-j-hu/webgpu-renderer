#include "noworry/scene/scene.h"

Scene::Scene(Renderer& renderer)
    : m_renderer(&renderer)
{
}

Model& Scene::add_model(const Mesh& mesh, Material& mat)
{
    m_models.push_back(
        std::make_unique<Model>(
            m_renderer->device(),
            m_renderer->texture_mesh_effect(),
            mesh,
            mat));
    return *m_models[m_models.size() - 1];
}
