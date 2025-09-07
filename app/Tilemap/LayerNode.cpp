#include "LayerNode.h"
#include "noworry/grid.h"
#include "noworry/Gfx3D/ModelInstance.h"

#include <utility>
#include "glm/ext/matrix_transform.hpp"

LayerNode::LayerNode(AppState& app_state)
    : m_app_state(&app_state),
      m_grid_mesh(
          create_grid(app_state.renderer().device(),
                      glm::vec3(0, 0, 0),
                      16,
                      16,
                      glm::vec3(16, 0, 0),
                      glm::vec3(0, 16, 0)))
{
    m_model = std::make_unique<DynamicModel>();
    m_instance = DynamicModelInstance(*m_model);
}

LayerNode::LayerNode(LayerNode&& other)
{
    m_app_state = nullptr;
    *this = std::move(other);
}

LayerNode& LayerNode::operator=(LayerNode&& other)
{
    std::swap(m_app_state, other.m_app_state);
    std::swap(m_model, other.m_model);
    std::swap(m_grid_mesh, other.m_grid_mesh);
    std::swap(m_instance, other.m_instance);
    return *this;
}

void LayerNode::render(Frame& frame)
{
    frame.add(*m_grid_mesh, m_app_state->wireframe_material(), glm::mat4(1));

    update();

    m_model->flush(frame.renderer().device());
    m_instance->render(frame);
}

void LayerNode::update()
{
    m_model->reset();

    auto& project = m_app_state->project();
    for (int i = 0; i < project.layer_count(); ++i) {
        auto& layer = project.layer_at(i);
        for (int y = 0; y < 16; ++y) {
            for (int x = 0; x < 16; ++x) {
                auto& opt = layer.at(x, y);
                if (!opt) {
                    continue;
                }

                auto& inst = opt.value();
                if (!inst.def().model_data) {
                    continue;
                }

                glm::mat4 transform =
                    glm::translate(glm::mat4(1), glm::vec3(x, y, inst.z()));

                m_model->add_model(**inst.def().model_data, transform);
            }
        }
    }
}
