#ifndef RENDERER_H
#define RENDERER_H

#include "camera.h"
#include "flatmesheffect.h"
#include "flatmeshpipeline.h"
#include "mesheffect.h"
#include "material.h"
#include "mesh.h"
#include "model.h"
#include "texture.h"
#include "texturemesheffect.h"
#include "texturemeshpipeline.h"
#include <memory>
#include <vector>

class Renderer
{
public:
    Renderer(WGPUDevice device, int width, int height);

    virtual ~Renderer();

    WGPUDevice device() { return m_device; }

    FlatMeshEffect& flat_mesh_effect() { return m_flat_effect; }

    FlatMeshPipeline& flat_mesh_pipeline() { return m_flat_pipeline; }

    TextureMeshEffect& texture_mesh_effect() { return m_effect; }

    TextureMeshPipeline& texture_mesh_pipeline() { return m_pipeline; }

    WGPUSampler default_sampler() { return m_sampler; }

    Camera& camera() { return m_camera; }

    void render(WGPUTextureView view);

    void resize(int width, int height);

    void set_clear_color(WGPUColor);

    void add_model(Model& model)
    {
        m_models.push_back(&model);
    }
private:
    WGPUColor m_clear_color;
    int m_width;
    int m_height;
    Texture m_depth_texture;

    WGPUDevice m_device;
    WGPUSampler m_sampler;
    FlatMeshEffect m_flat_effect;
    FlatMeshPipeline m_flat_pipeline;
    TextureMeshEffect m_effect;
    TextureMeshPipeline m_pipeline;
    Camera m_camera;

    std::vector<Model*> m_models;

    void create_depth_buffer(int width, int height);
    void do_render(WGPURenderPassEncoder encoder);
};

#endif
