#include "LayerNode.h"
#include "noworry/grid.h"
#include "noworry/Gfx3D/ModelInstance.h"

#include <utility>

LayerNode::LayerNode(AppState& app_state, const Layer& layer)
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
    m_thumbnail.set_clear_color(app_state.background_color());

    layer.listenable().add_listener(*this);

    update();
}

LayerNode::LayerNode(LayerNode&& other)
    : m_app_state {other.m_app_state},
      m_model(std::move(other.m_model)),
      m_grid_mesh(std::move(other.m_grid_mesh)),
      m_instance(std::move(other.m_instance)),
      m_layer {other.m_layer}
{
    if (m_layer != nullptr) {
        m_layer->listenable().remove_listener(other);
        m_layer->listenable().add_listener(*this);
    }
}

LayerNode& LayerNode::operator=(LayerNode&& other)
{
    if (m_layer != nullptr) {
        m_layer->listenable().remove_listener(*this);
    }

    m_app_state = std::exchange(other.m_app_state, nullptr);
    m_model = std::move(other.m_model);
    m_grid_mesh = std::move(other.m_grid_mesh);
    m_instance = std::move(other.m_instance);
    m_layer = std::exchange(other.m_layer, nullptr);

    if (m_layer != nullptr) {
        m_layer->listenable().remove_listener(other);
        m_layer->listenable().add_listener(*this);
    }

    return *this;
}

LayerNode::~LayerNode()
{
    if (m_layer != nullptr) {
        m_layer->listenable().remove_listener(*this);
    }
}

const Texture& LayerNode::thumbnail()
{
    return m_thumbnail.texture();
}

void LayerNode::layer_changed()
{
    update();
}

void LayerNode::render(Frame& frame)
{
    m_instance->render(frame);
}

void LayerNode::update()
{
    m_model->reset();
    m_layer->fill_model(*m_model);
    m_model->flush(m_app_state->renderer().device());
    m_app_state->thumbnail_util().capture(m_thumbnail, *m_instance);
}
