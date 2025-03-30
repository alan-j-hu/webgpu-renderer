#ifndef EFFECT_H
#define EFFECT_H

#include <glm/mat4x4.hpp>
#include <webgpu/webgpu.h>

struct Vertex
{
    float x, y, z;
    float u, v;
};

struct CameraData
{
    glm::mat4 viewproj;
};

struct ModelData
{
    glm::mat4 transform;
};

class Effect
{
public:
    Effect(WGPUDevice device);
    Effect(const Effect&) = delete;
    Effect& operator=(const Effect&) = delete;
    virtual ~Effect();

    WGPUShaderModule vertex_shader() { return m_vertex_shader; }
    WGPUShaderModule fragment_shader() { return m_fragment_shader; }
    WGPUPipelineLayout pipeline_layout() { return m_pipeline_layout; }

    WGPUBindGroupLayout camera_layout() { return m_camera_layout; }
    WGPUBindGroupLayout material_layout() { return m_material_layout; }
    WGPUBindGroupLayout model_layout() { return m_model_layout; }

    WGPUBindGroup create_camera_group(WGPUDevice, WGPUBuffer buffer) const;

    WGPUBindGroup create_material_group(
        WGPUDevice, WGPUTextureView, WGPUSampler) const;

    WGPUBindGroup create_model_group(
        WGPUDevice,
        WGPUBuffer transform,
        WGPUBuffer vertices,
        int tri_count) const;
private:

    WGPUShaderModule m_vertex_shader;
    WGPUShaderModule m_fragment_shader;
    WGPUBindGroupLayout m_camera_layout;
    WGPUBindGroupLayout m_material_layout;
    WGPUBindGroupLayout m_model_layout;
    WGPUPipelineLayout m_pipeline_layout;
};

#endif
