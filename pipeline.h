#ifndef PIPELINE_H
#define PIPELINE_H

#include <webgpu/webgpu.h>

struct Vertex
{
    float x, y, z;
    float _padding;
    float r, g, b, a;
};

struct Camera
{
    float viewproj[4][4];
};

struct Model
{
    float transform[4][4];
};

class Pipeline
{
public:
    Pipeline(WGPUDevice device);

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
