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
}

LayerNode::LayerNode(LayerNode&& other)
{
    m_app_state = nullptr;
    *this = std::move(other);
}

LayerNode& LayerNode::operator=(LayerNode&& other)
{
    std::swap(m_app_state, other.m_app_state);
    std::swap(m_grid_mesh, other.m_grid_mesh);
    return *this;
}

void LayerNode::render(Frame& frame)
{
    frame.add(*m_grid_mesh, m_app_state->wireframe_material(), glm::mat4(1));

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

                Transform transform;
                transform.set_translation(glm::vec3(x, y, inst.z()));

                if (!inst.def().model) {
                    continue;
                }

                ModelInstance m(**inst.def().model);
                m.transform() =
                    glm::translate(m.transform(), glm::vec3(x, y, inst.z()));
                m.render(frame);
            }
        }
    }
}
