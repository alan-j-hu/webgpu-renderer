#ifndef LAYER_NODE_H
#define LAYER_NODE_H

#include "../AppState.h"
#include "noworry/Gfx3D/Renderable.h"
#include <optional>

class LayerNode : public Renderable
{
public:
    LayerNode(AppState&);
    LayerNode(LayerNode&&);
    LayerNode& operator=(LayerNode&&);

    virtual void render(Frame& frame) override;

private:
    AppState* m_app_state;
    std::optional<BasicMesh> m_grid_mesh;
};

#endif
