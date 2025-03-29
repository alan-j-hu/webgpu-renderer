#ifndef RENDERER_H
#define RENDERER_H

#include "camera.h"
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

    Camera& camera() { return m_camera; }

    void render(WGPUTextureView view);

    void resize(int width, int height);

    Texture& add_texture(const std::filesystem::path& path);

    Material& add_material(const Texture&);

    Mesh& add_mesh(Vertex* vertices, std::size_t count, Material& material);

    Model& add_model(const Mesh& mesh);

    void set_clear_color(WGPUColor);
private:
    WGPUColor m_clear_color;
    int m_width;
    int m_height;
    WGPUTexture m_depth_texture;
    WGPUTextureView m_depth_texture_view;

    WGPUDevice m_device;
    WGPUSampler m_sampler;
    Pipeline m_pipeline;
    Camera m_camera;
    std::vector<std::unique_ptr<Texture>> m_textures;
    std::vector<std::unique_ptr<Material>> m_materials;
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::unique_ptr<Model>> m_models;

    void create_depth_buffer(int width, int height);
    void do_render(WGPURenderPassEncoder encoder);
};

#endif
