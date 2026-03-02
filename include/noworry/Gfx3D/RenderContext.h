#ifndef SCENE_H
#define SCENE_H

#include "Renderable.h"
#include "../Pipeline/MeshVertexShader.h"
#include "../camera/camera.h"

class Material;
class Mesh;
class Renderer;

class RenderContext
{
public:
    RenderContext(Renderer&);
    RenderContext(const RenderContext&) = delete;
    RenderContext& operator=(const RenderContext&) = delete;
    RenderContext(RenderContext&&);
    RenderContext& operator=(RenderContext&&);
    virtual ~RenderContext();

    void setup_camera(Camera&);

    void add(const Mesh&, const Material&, const glm::mat4&);

    WGPUBindGroup bind_group() { return m_bind_group; }

private:
    Renderer* m_renderer;

    GlobalUniforms m_uniforms;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;
};

#endif
