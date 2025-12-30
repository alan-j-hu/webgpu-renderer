#include "LevelNode.h"

LevelNode::LevelNode(AppState& app_state, const Level& level)
    : m_app_state(&app_state),
      m_level(&level)
{
    auto& project = app_state.project();
    for (int i = 0; i < level.layer_count(); ++i) {
        add_layer(i);
    }
}

void LevelNode::render(Frame& frame)
{
    for (auto& layer_ptr : m_layer_nodes) {
        layer_ptr->render(frame);
    }
}

LayerNode& LevelNode::layer_at(int i)
{
    return *m_layer_nodes.at(i);
}

void LevelNode::add_layer(int idx)
{
    auto ptr =
        std::make_unique<LayerNode>(*m_app_state, m_level->layer_at(idx));
    m_layer_nodes.insert(m_layer_nodes.begin() + idx, std::move(ptr));
}

void LevelNode::remove_layer(int idx)
{
    m_layer_nodes.erase(m_layer_nodes.begin() + idx);
}
