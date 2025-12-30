#include "LevelNode.h"

LevelNode::LevelListener::LevelListener(LevelNode& level_node)
    : m_level_node(&level_node)
{
}

void LevelNode::LevelListener::add_layer(Layer& layer, int index)
{
    m_level_node->add_layer(layer, index);
}

void LevelNode::LevelListener::remove_layer(int index)
{
    m_level_node->remove_layer(index);
}

LevelNode::LevelNode(AppState& app_state, const LayerLocation& location)
    : m_app_state(&app_state),
      m_level_listener(*this)
{
    auto& project = app_state.project();
    auto& level = project.level_at(location);
    for (int i = 0; i < level.layer_count(); ++i) {
        add_layer(level.layer_at(i), i);
    }
}

void LevelNode::render(Frame& frame)
{
    for (auto& layer_ptr : m_layer_nodes) {
        layer_ptr->render(frame);
    }
}

Level::Listener& LevelNode::listener()
{
    return m_level_listener;
}

LayerNode& LevelNode::layer_at(int i)
{
    return *m_layer_nodes.at(i);
}

void LevelNode::add_layer(const Layer& layer, int index)
{
    auto ptr = std::make_unique<LayerNode>(*m_app_state, layer);
    m_layer_nodes.insert(m_layer_nodes.begin() + index, std::move(ptr));
}

void LevelNode::remove_layer(int index)
{
    m_layer_nodes.erase(m_layer_nodes.begin() + index);
}
