#ifndef LEVEL_NODE_H
#define LEVEL_NODE_H

#include "LayerNode.h"
#include "../Project.h"
#include <memory>

class AppState;

class LevelNode : public Renderable
{
public:
    LevelNode(AppState&, const Level&);
    virtual ~LevelNode() = default;

    virtual void render(Frame& frame) override;

    LayerNode& layer_at(int idx);

    void add_layer(int idx);

    void remove_layer(int idx);

private:
    AppState* m_app_state;
    const Level* m_level;
    std::vector<std::unique_ptr<LayerNode>> m_layer_nodes;
};

#endif
