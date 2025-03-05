#ifndef PIPELINE_H
#define PIPELINE_H

#include <glm/mat4x4.hpp>
#include <webgpu/webgpu.h>

struct Vertex
{
    float x, y, z;
    float _padding;
    float r, g, b, a;
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

    WGPUBindGroupLayout camera_layout() { return m_layout0; }
    WGPUBindGroupLayout model_layout() { return m_layout1; }

    WGPUBindGroup create_camera_group(WGPUBuffer buffer);

    WGPUBindGroup create_model_group(WGPUBuffer buffer);

private:
    WGPUDevice m_device;
    WGPURenderPipeline m_pipeline;
    WGPUBindGroupLayout m_layout0;
    WGPUBindGroupLayout m_layout1;
};

#endif
