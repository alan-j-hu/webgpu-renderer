#ifndef LAYER_NODE_H
#define LAYER_NODE_H

#include "../AppState.h"
#include "noworry/rendertarget.h"
#include "noworry/Gfx3D/DynamicModel.h"
#include "noworry/Gfx3D/DynamicModelInstance.h"
#include "noworry/Gfx3D/Renderable.h"
#include <memory>
#include <optional>

class LayerNode;

class LayerNode : public Renderable
{
public:
    LayerNode(AppState&, const Layer&);
    LayerNode(LayerNode&&);
    LayerNode& operator=(LayerNode&&);
    ~LayerNode();

    const Texture& thumbnail() { return m_thumbnail.texture(); }

    void update();

    virtual void render(Frame& frame) override;

private:
    AppState* m_app_state;
    const Layer* m_layer;
    std::unique_ptr<DynamicModel> m_model;
    std::optional<DynamicModelInstance> m_instance;
    std::optional<BasicMesh> m_grid_mesh;
    RenderTarget m_thumbnail;
};

#endif
