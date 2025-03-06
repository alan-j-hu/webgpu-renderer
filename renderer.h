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

    Camera& camera() { return m_camera; }

    void render(WGPUTextureView view);

    Mesh& add_mesh(Vertex* vertices, std::size_t count);

    Model& add_model(const Mesh& mesh);
private:
    WGPUDevice m_device;
    Pipeline m_pipeline;
    Camera m_camera;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::unique_ptr<Model>> m_models;

    void do_render(WGPURenderPassEncoder encoder);
};

#endif
