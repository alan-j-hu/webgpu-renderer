#ifndef UNIFORM_LAYOUT_H
#define UNIFORM_LAYOUT_H

#include <glm/mat4x4.hpp>
#include <webgpu/webgpu.h>

struct CameraData
{
    glm::mat4 viewproj;
};

struct Uniforms
{
    CameraData camera;
};

class UniformLayout
{
public:
    UniformLayout(WGPUDevice device);

    WGPUBindGroupLayout layout() { return m_layout; }

    WGPUBindGroup create_bind_group(WGPUDevice, WGPUBuffer buffer);

private:
    WGPUBindGroupLayout m_layout;
};

#endif
