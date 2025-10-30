#ifndef SCENE_H
#define SCENE_H

#include "Node.h"
#include "renderobject.h"
#include "../Pipeline/MeshVertexShader.h"
#include "../renderer.h"
#include "../camera/camera.h"
#include "../Gfx3D/Renderable.h"

class Scene : public Node
{
public:
    Scene(Renderer&);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&);
    Scene& operator=(Scene&&);
    virtual ~Scene();

    void render(Frame&, Camera&);

    WGPUBindGroup bind_group() { return m_bind_group; }

private:
    Renderer* m_renderer;

    GlobalUniforms m_uniforms;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;
};

#endif
