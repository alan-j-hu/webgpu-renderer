#ifndef LEVEL_NODE_H
#define LEVEL_NODE_H

#include "LayerNode.h"
#include "../Project.h"
#include <memory>

class AppState;

class LevelNode : public Renderable
{
public:
    LevelNode(AppState&);
    virtual ~LevelNode() = default;

    virtual void render(Frame& frame) override;

    LayerNode& layer_at(int i);

    Level::Listener& listener();

private:
    class LevelListener : public Level::Listener
    {
    public:
        LevelListener(LevelNode&);

        virtual void add_layer(Layer&, int index) override;
        virtual void remove_layer(int index) override;

    private:
        LevelNode* m_level_node;
    };

    AppState* m_app_state;
    std::vector<std::unique_ptr<LayerNode>> m_layer_nodes;
    LevelListener m_level_listener;

    void add_layer(Layer&, int index);

    void remove_layer(int index);
};

#endif
