#ifndef RENDERER_H
#define RENDERER_H

#include "mesh.h"
#include "texture.h"
#include "material/flatmesheffect.h"
#include "material/mesheffect.h"
#include "material/material.h"
#include "material/texturemesheffect.h"
#include "material/uniformlayout.h"
#include <memory>
#include <vector>

class Camera;
class RenderTarget;
class Scene;

class Renderer
{
public:
    Renderer(WGPUDevice device);

    virtual ~Renderer();

    WGPUDevice device() { return m_device; }

    FlatMeshEffect& flat_mesh_effect() { return m_flat_effect; }

    TextureMeshEffect& texture_mesh_effect() { return m_effect; }

    WGPUSampler default_sampler() { return m_sampler; }

    UniformLayout& uniform_layout() { return m_uniform_layout; }

    void render(RenderTarget&, Scene& scene);

private:
    WGPUDevice m_device;
    UniformLayout m_uniform_layout;
    WGPUSampler m_sampler;
    FlatMeshEffect m_flat_effect;
    TextureMeshEffect m_effect;

    void do_render(WGPURenderPassEncoder encoder);
};

#endif
