#ifndef LAYER_NODE_H
#define LAYER_NODE_H

#include "../AppState.h"
#include "noworry/Gfx3D/DynamicModel.h"
#include "noworry/Gfx3D/DynamicModelInstance.h"
#include "noworry/Gfx3D/Renderable.h"
#include <memory>
#include <optional>

class LayerNode;

class LayerNode : public Renderable
{
public:
    LayerNode(AppState&, Layer&);
    LayerNode(LayerNode&&);
    LayerNode& operator=(LayerNode&&);

    virtual void render(Frame& frame) override;

private:
    class ChangeListener : public Listener<const Layer&>
    {
    public:
        ChangeListener(LayerNode&);
        ChangeListener(ChangeListener&&);
        ChangeListener& operator=(ChangeListener&&);

        virtual void operator()(const Layer&) override;

    private:
        LayerNode* m_node;
    };

    AppState* m_app_state;
    const Layer* m_layer;
    std::unique_ptr<DynamicModel> m_model;
    std::optional<DynamicModelInstance> m_instance;
    std::optional<BasicMesh> m_grid_mesh;
    std::unique_ptr<ChangeListener> m_change_listener;

    void update();
};

#endif
