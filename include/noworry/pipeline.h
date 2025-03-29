#ifndef PIPELINE_H
#define PIPELINE_H

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

class Pipeline
{
public:
    Pipeline(WGPUDevice device);
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    ~Pipeline();

    WGPURenderPipeline pipeline() { return m_pipeline; }

    WGPUBindGroupLayout camera_layout() { return m_camera_layout; }
    WGPUBindGroupLayout material_layout() { return m_material_layout; }
    WGPUBindGroupLayout model_layout() { return m_model_layout; }

    WGPUBindGroup create_camera_group(WGPUBuffer buffer) const;

    WGPUBindGroup create_material_group(WGPUTextureView, WGPUSampler) const;

    WGPUBindGroup create_model_group(
        WGPUBuffer transform, WGPUBuffer vertices, int tri_count) const;

private:
    WGPUDevice m_device;
    WGPURenderPipeline m_pipeline;
    WGPUBindGroupLayout m_camera_layout;
    WGPUBindGroupLayout m_material_layout;
    WGPUBindGroupLayout m_model_layout;
};

#endif
