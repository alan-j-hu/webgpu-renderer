#include "noworry/Gfx3D/RenderContext.h"
#include "noworry/renderer.h"
#include <utility>

RenderContext::RenderContext(Renderer& renderer)
    : m_renderer(&renderer)
{
    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(CameraData);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(renderer.device(), &buffer_desc);

    m_bind_group = renderer
        .mesh_vertex_shader().create_global_group(renderer.device(), m_buffer);
}

RenderContext::RenderContext(RenderContext&& other)
    : m_renderer(std::exchange(other.m_renderer, nullptr)),
      m_uniforms {other.m_uniforms},
      m_buffer(std::exchange(other.m_buffer, nullptr)),
      m_bind_group(std::exchange(other.m_bind_group, nullptr))
{
}

RenderContext& RenderContext::operator=(RenderContext&& other)
{
    if (m_bind_group != nullptr) {
        wgpuBindGroupRelease(m_bind_group);
    }
    if (m_buffer != nullptr) {
        wgpuBufferRelease(m_buffer);
    }

    m_renderer = std::exchange(other.m_renderer, nullptr);
    m_uniforms = std::move(other.m_uniforms);
    m_buffer = std::exchange(other.m_buffer, nullptr);
    m_bind_group = std::exchange(other.m_bind_group, nullptr);

    return *this;
}

RenderContext::~RenderContext()
{
    if (m_bind_group != nullptr) {
        wgpuBindGroupRelease(m_bind_group);
    }
    if (m_buffer != nullptr) {
        wgpuBufferRelease(m_buffer);
    }
}

void RenderContext::setup_camera(Camera& camera)
{
    camera.update_matrix(&m_uniforms.camera);
    WGPUQueue queue = wgpuDeviceGetQueue(m_renderer->device());
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_uniforms, sizeof(GlobalUniforms));
}

void RenderContext::add(const Mesh& mesh,
                        const Material& material,
                        const glm::mat4& transform)
{
    if (mesh.vertex_count() != 0 && mesh.index_count() != 0) {
        m_renderer->batcher().enqueue_parts(mesh, material, transform);
    }
}