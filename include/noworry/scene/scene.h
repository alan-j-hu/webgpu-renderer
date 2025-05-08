#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "renderobject.h"
#include "../mesh.h"
#include "../renderer.h"
#include "../material/material.h"
#include "../material/uniformlayout.h"
#include <memory>
#include <vector>

class Scene
{
public:
    using RenderObjectIterator =
        std::vector<std::unique_ptr<RenderObject>>::iterator;

    Scene(Renderer&);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&);
    Scene& operator=(Scene&&);
    virtual ~Scene();

    RenderObject& add_renderobject(const Mesh& mesh, Material& mat);

    RenderObjectIterator renderobjects_begin()
    { return m_renderobjects.begin(); }

    RenderObjectIterator renderobjects_end()
    { return m_renderobjects.end(); }

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
    std::vector<std::unique_ptr<RenderObject>> m_renderobjects;
};

#endif
