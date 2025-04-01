#ifndef RENDERER_H
#define RENDERER_H

#include "camera.h"
#include "effect.h"
#include "material.h"
#include "mesh.h"
#include "model.h"
#include "pipeline.h"
#include "texture.h"
#include <memory>
#include <vector>

class Renderer
{
public:
    Renderer(WGPUDevice device, int width, int height);

    virtual ~Renderer();

    WGPUDevice device() { return m_device; }

    Effect& texture_mesh_effect() { return m_effect; }

    Pipeline& texture_mesh_pipeline() { return m_pipeline; }

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
    WGPUTexture m_depth_texture;
    WGPUTextureView m_depth_texture_view;

    WGPUDevice m_device;
    WGPUSampler m_sampler;
    Effect m_effect;
    Pipeline m_pipeline;
    Camera m_camera;

    std::vector<Model*> m_models;

    void create_depth_buffer(int width, int height);
    void do_render(WGPURenderPassEncoder encoder);
};

#endif
