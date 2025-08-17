#include "LayerNode.h"
#include "noworry/grid.h"
#include <utility>

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
    Transform transform;
    frame.add(transform, *m_grid_mesh, m_app_state->wireframe_material());

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
                ResolvedTile resolved = m_app_state->resolve_tile(inst.def());
                if (resolved.mesh == nullptr) {
                    continue;
                }
                Material& material = resolved.material == nullptr
                    ? m_app_state->default_material()
                    : *resolved.material;

                Transform transform;
                transform.set_translation(glm::vec3(x, y, inst.z()));
                frame.add(transform, resolved.mesh->mesh(), material);
            }
        }
    }
}
