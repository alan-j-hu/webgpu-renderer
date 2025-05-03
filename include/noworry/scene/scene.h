#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "model.h"
#include "../mesh.h"
#include "../renderer.h"
#include "../material/material.h"
#include "../material/uniformlayout.h"
#include <memory>
#include <vector>

class Scene
{
public:
    using ModelIterator = std::vector<std::unique_ptr<Model>>::iterator;

    Scene(Renderer&);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&);
    Scene& operator=(Scene&&);
    virtual ~Scene();

    Model& add_model(const Mesh& mesh, Material& mat);

    ModelIterator models_begin() { return m_models.begin(); }
    ModelIterator models_end() { return m_models.end(); }

    void update();

    Camera& current_camera() { return *m_cameras[m_camera]; }

    WGPUBindGroup bind_group() { return m_bind_group; }

private:
    bool m_moved = false;
    Renderer* m_renderer;
    int m_camera = 0;

    Uniforms m_uniforms;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;

    std::vector<std::unique_ptr<Camera>> m_cameras;
    std::vector<std::unique_ptr<Model>> m_models;
};

#endif
