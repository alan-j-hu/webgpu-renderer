#ifndef LEVEL_NODE_H
#define LEVEL_NODE_H

#include "LayerNode.h"
#include "../../Project.h"
#include <memory>

class AppState;

class LevelNode : public Level::Listener, public Renderable
{
public:
    LevelNode(AppState&, const Level&);

    virtual ~LevelNode();

    virtual void layer_added(int idx) override;

    virtual void layer_removed(Layer&, int idx) override;

    virtual void layer_reordered(Layer&, int old_idx, int new_idx) override;

    virtual void render(RenderContext&) override;

    LayerNode& layer_at(int idx);

    void add_layer(int idx);

    void remove_layer(int idx);

    void reorder_layer(int idx, int delta);

private:
    AppState* m_app_state;
    const Level* m_level;
    std::vector<std::unique_ptr<LayerNode>> m_layer_nodes;
};

#endif
