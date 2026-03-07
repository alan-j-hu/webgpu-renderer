#include "LevelNode.h"
#include "../../Util.h"

LevelNode::LevelNode(AppState& app_state, const Level& level)
    : m_app_state(&app_state),
      m_level(&level)
{
    auto& project = app_state.project();
    level.listenable().add_listener(*this);
    for (int i = 0; i < level.layer_count(); ++i) {
        add_layer(i);
    }
}

LevelNode::~LevelNode()
{
    m_level->listenable().remove_listener(*this);
}

void LevelNode::layer_added(int idx)
{
    add_layer(idx);
}

void LevelNode::layer_removed(Layer&, int idx)
{
    remove_layer(idx);
}

void LevelNode::layer_reordered(Layer&, int old_idx, int new_idx)
{
    reorder_layer(old_idx, new_idx);
}

void LevelNode::render(RenderContext& ctx)
{
    for (auto& layer_ptr : m_layer_nodes) {
        layer_ptr->renderable().render(ctx);
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

void LevelNode::reorder_layer(int idx, int delta)
{
    reorder(m_layer_nodes, idx, delta);
}
