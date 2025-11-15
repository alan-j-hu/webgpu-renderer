#include "LayerNode.h"
#include "noworry/grid.h"
#include "noworry/Gfx3D/ModelInstance.h"

#include <utility>
#include "glm/ext/matrix_transform.hpp"

LayerNode::ChangeListener::ChangeListener(LayerNode& node)
    : m_node(&node)
{
}

LayerNode::ChangeListener::ChangeListener(LayerNode::ChangeListener&& other)
{
    m_node = nullptr;
    *this = std::move(other);
}

LayerNode::ChangeListener&
LayerNode::ChangeListener::operator=(LayerNode::ChangeListener&& other)
{
    std::swap(m_node, other.m_node);
    return *this;
}

void LayerNode::ChangeListener::notify(const Layer& layer)
{
    m_node->update();
}

LayerNode::LayerNode(AppState& app_state, Layer& layer)
    : m_app_state(&app_state),
      m_grid_mesh(
          create_grid(app_state.renderer().device(),
                      glm::vec3(0, 0, 0),
                      16,
                      16,
                      glm::vec3(16, 0, 0),
                      glm::vec3(0, 16, 0))),
      m_layer(&layer),
      m_thumbnail(app_state.renderer().device(), 100, 100)
{
    m_model = std::make_unique<DynamicModel>();
    m_instance = DynamicModelInstance(*m_model);
    m_change_listener = std::make_unique<LayerNode::ChangeListener>(*this);
    m_thumbnail.set_clear_color(app_state.background_color());

    update();

    layer.listenable().add_listener(*m_change_listener);
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
    std::swap(m_change_listener, other.m_change_listener);
    return *this;
}

LayerNode::~LayerNode()
{
    m_layer->listenable().remove_listener(*m_change_listener);
}

void LayerNode::render(Frame& frame)
{
    m_instance->render(frame);
}

void LayerNode::update()
{
    m_model->reset();

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            auto& opt = m_layer->at(x, y);
            if (!opt) {
                continue;
            }

            auto& inst = opt.value();
            if (!inst.def()->model_data) {
                continue;
            }

            glm::mat4 transform =
                glm::translate(glm::mat4(1), glm::vec3(x, y, inst.z()));

            m_model->add_model(**inst.def()->model_data, transform);
        }
    }

    m_model->flush(m_app_state->renderer().device());

    m_app_state->thumbnail_capture().capture(m_thumbnail, *m_instance);
}
