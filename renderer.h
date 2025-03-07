#ifndef RENDERER_H
#define RENDERER_H

#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "pipeline.h"
#include <memory>
#include <vector>

class Renderer
{
public:
    Renderer(WGPUDevice device, int width, int height);

    virtual ~Renderer();

    Camera& camera() { return m_camera; }

    void render(WGPUTextureView view);

    void resize(int width, int height);

    Mesh& add_mesh(Vertex* vertices, std::size_t count);

    Model& add_model(const Mesh& mesh);

    void set_clear_color(WGPUColor);
private:
    WGPUColor m_clear_color;
    int m_width;
    int m_height;
    WGPUTexture m_depth_texture;
    WGPUTextureView m_depth_texture_view;

    WGPUDevice m_device;
    Pipeline m_pipeline;
    Camera m_camera;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::unique_ptr<Model>> m_models;

    void create_depth_buffer(int width, int height);
    void do_render(WGPURenderPassEncoder encoder);
};

#endif
