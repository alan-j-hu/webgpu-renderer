#ifndef SCENE_H
#define SCENE_H

#include "Node.h"
#include "renderobject.h"
#include "../Pipeline/MeshVertexShader.h"
#include "../renderer.h"
#include "../camera/camera.h"
#include "../Gfx3D/Renderable.h"

class Scene : public Node, public Renderable
{
public:
    Scene(Renderer&, Camera&);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&);
    Scene& operator=(Scene&&);
    virtual ~Scene();

    void set_camera(Camera& camera);

    void render(Frame&);

    WGPUBindGroup bind_group() { return m_bind_group; }

private:
    Renderer* m_renderer;

    GlobalUniforms m_uniforms;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;

    Camera* m_camera;
};

#endif
