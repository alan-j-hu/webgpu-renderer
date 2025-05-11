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

    Scene(Renderer&);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&);
    Scene& operator=(Scene&&);
    virtual ~Scene();

    void update(Renderer&);

    Camera& camera() { return *m_camera; }

    WGPUBindGroup bind_group() { return m_bind_group; }

private:
    bool m_moved = false;
    Renderer* m_renderer;

    Uniforms m_uniforms;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;

    std::unique_ptr<Camera> m_camera;
};

#endif
