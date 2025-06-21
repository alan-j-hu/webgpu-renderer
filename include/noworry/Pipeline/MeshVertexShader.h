#ifndef MESHVERTEXSHADER
#define MESHVERTEXSHADER

#include <array>
#include <glm/mat4x4.hpp>
#include <webgpu/webgpu.h>

struct CameraData
{
    glm::mat4 viewproj;
};

struct GlobalUniforms
{
    CameraData camera;
};

struct Vertex
{
    float x, y, z;
    float u, v;

    static const std::array<WGPUVertexAttribute, 5>& attributes();
};

struct ModelUniforms
{
    glm::mat4 transform;
};

class MeshVertexShader
{
public:
    MeshVertexShader(WGPUDevice);
    virtual ~MeshVertexShader();

    WGPUShaderModule vertex_shader() { return m_vertex_shader; }
    WGPUBindGroupLayout global_layout() { return m_global_layout; }
    WGPUBindGroupLayout model_layout() { return m_model_layout; }

    WGPUBindGroup create_global_group(WGPUDevice, WGPUBuffer transform) const;
    WGPUBindGroup create_model_group(WGPUDevice, WGPUBuffer transform) const;

private:
    WGPUShaderModule m_vertex_shader;
    WGPUBindGroupLayout m_global_layout;
    WGPUBindGroupLayout m_model_layout;
};

#endif
