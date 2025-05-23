#ifndef SCENE_H
#define SCENE_H

#include "renderobject.h"
#include "../mesh.h"
#include "../renderer.h"
#include "../camera/camera.h"
#include "../material/material.h"
#include "../material/uniformlayout.h"
#include <memory>
#include <vector>

class Scene
{
public:

    Scene(Renderer&, Camera&);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&);
    Scene& operator=(Scene&&);
    virtual ~Scene();

    void set_camera(Camera& camera);

    void update(Renderer&);

    WGPUBindGroup bind_group() { return m_bind_group; }

private:
    bool m_moved = false;
    Renderer* m_renderer;

    Uniforms m_uniforms;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;

    Camera* m_camera;
};

#endif
