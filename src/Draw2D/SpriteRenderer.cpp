#include "noworry/Draw2D/SpriteRenderer.h"
#include <cstdint>
#include <glm/mat4x4.hpp>

SpriteRenderer::SpriteRenderer(WGPUDevice device, std::size_t max)
    : m_capacity(max),
      m_device(device),
      m_pipeline(device),
      m_vertex_buffer(nullptr),
      m_index_buffer(nullptr)
{
    resize(max);

    WGPUBufferDescriptor viewproj_buffer_desc = { 0 };
    viewproj_buffer_desc.nextInChain = nullptr;
    viewproj_buffer_desc.usage =
        WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    viewproj_buffer_desc.size = 64;
    viewproj_buffer_desc.mappedAtCreation = false;
    m_viewproj_buffer = wgpuDeviceCreateBuffer(device, &viewproj_buffer_desc);

    m_global_bind_group =
        m_pipeline.create_global_bind_group(device, m_viewproj_buffer);
}

SpriteRenderer::~SpriteRenderer()
{
    if (m_index_buffer != nullptr) {
        wgpuBufferRelease(m_index_buffer);
    }
    if (m_vertex_buffer != nullptr) {
        wgpuBufferRelease(m_vertex_buffer);
    }
    if (m_viewproj_buffer != nullptr) {
        wgpuBufferRelease(m_viewproj_buffer);
    }
    if (m_global_bind_group != nullptr) {
        wgpuBindGroupRelease(m_global_bind_group);
    }
}

void SpriteRenderer::resize(int size)
{
    if (m_index_buffer != nullptr) {
        wgpuBufferRelease(m_index_buffer);
    }
    if (m_vertex_buffer != nullptr) {
        wgpuBufferRelease(m_vertex_buffer);
    }

    const std::size_t ibuffer_size = 6 * sizeof(std::uint16_t) * size;
    std::vector<std::uint16_t> indices;
    indices.reserve(6 * size);
    for (int i = 0; i < size; ++i) {
        int index = 4 * i;
        indices.push_back(index);
        indices.push_back(index + 1);
        indices.push_back(index + 2);
        indices.push_back(index);
        indices.push_back(index + 2);
        indices.push_back(index + 3);
    }

    WGPUBufferDescriptor vbuffer_desc = { 0 };
    vbuffer_desc.nextInChain = nullptr;
    vbuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
    vbuffer_desc.size = 4 * sizeof(glm::vec4) * size;
    vbuffer_desc.mappedAtCreation = false;
    m_vertex_buffer = wgpuDeviceCreateBuffer(m_device, &vbuffer_desc);

    WGPUBufferDescriptor ibuffer_desc = { 0 };
    ibuffer_desc.nextInChain = nullptr;
    ibuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
    ibuffer_desc.size = ibuffer_size;
    ibuffer_desc.mappedAtCreation = false;
    m_index_buffer = wgpuDeviceCreateBuffer(m_device, &ibuffer_desc);

    WGPUQueue queue = wgpuDeviceGetQueue(m_device);
    wgpuQueueWriteBuffer(queue, m_index_buffer, 0,
                         indices.data(), ibuffer_size);

    m_capacity = size;
}

void SpriteRenderer::begin(RenderTarget& target)
{
    m_current_draw_call.bind_group = nullptr;
    m_current_draw_call.begin = 0;
    m_frame = std::move(Frame2D(m_device, target));
}

void SpriteRenderer::end()
{
    if (!m_frame) {
        return;
    }

    m_current_draw_call.end = 6 * (m_draw_calls.size() + 1);
    m_draw_calls.push_back(m_current_draw_call);
    flush();
    m_current_draw_call.bind_group = nullptr;
    m_current_draw_call.begin = 0;
    m_frame->finish();
    m_frame.reset();
}

void SpriteRenderer::draw(
    const Spritesheet& spritesheet,
    const Region& dest,
    const Region& src)
{
    if (!m_frame) {
        return;
    }

    WGPUBindGroup bind_group = spritesheet.bind_group();
    if (m_current_draw_call.bind_group == nullptr) {
        m_current_draw_call.bind_group = bind_group;
    } else if (bind_group != m_current_draw_call.bind_group) {
        m_current_draw_call.end = 6 * (m_draw_calls.size() + 1);
        m_draw_calls.push_back(m_current_draw_call);

        m_current_draw_call.bind_group = spritesheet.bind_group();
        m_current_draw_call.begin = 6 * m_draw_calls.size();
    }

    m_vertices.push_back(glm::vec4(dest.x, dest.y,
                                   src.x, src.y));
    m_vertices.push_back(glm::vec4(dest.x, dest.y + dest.h,
                                   src.x, src.y + src.h));
    m_vertices.push_back(glm::vec4(dest.x + dest.w, dest.y + dest.h,
                                   src.x + src.w, src.y + src.h));
    m_vertices.push_back(glm::vec4(dest.x + dest.w, dest.y,
                                   src.x + src.w, src.y));
}

void SpriteRenderer::flush()
{
    if (!m_frame) {
        return;
    }
    if (m_draw_calls.size() == 0) {
        return;
    }

    if (m_vertices.size() > m_capacity) {
        resize(m_vertices.capacity());
    }

    WGPUQueue queue = wgpuDeviceGetQueue(m_device);
    wgpuQueueWriteBuffer(queue, m_vertex_buffer, 0, m_vertices.data(),
                         sizeof(glm::vec4) * m_vertices.size());

    m_vertices.clear();

    glm::mat4 viewproj(1);
    viewproj[0][0] = 2.0f / m_frame->target().width();
    viewproj[1][1] = -2.0f / m_frame->target().height();
    viewproj[3][0] = -1.0f;
    viewproj[3][1] = 1.0f;

    wgpuQueueWriteBuffer(queue, m_viewproj_buffer, 0, &viewproj,
                         sizeof(glm::mat4));

    for (int i = 0; i < m_draw_calls.size(); ++i) {
        DrawCall& draw_call = m_draw_calls[i];
        if (draw_call.begin == draw_call.end) {
            continue;
        }

        WGPURenderPassEncoder encoder = m_frame->begin_pass();

        wgpuRenderPassEncoderSetPipeline(encoder, m_pipeline.pipeline());
        wgpuRenderPassEncoderSetVertexBuffer(
            encoder, 0, m_vertex_buffer, 0,
            wgpuBufferGetSize(m_vertex_buffer));
        wgpuRenderPassEncoderSetIndexBuffer(
            encoder, m_index_buffer, WGPUIndexFormat_Uint16, 0,
            wgpuBufferGetSize(m_index_buffer));
        wgpuRenderPassEncoderSetBindGroup(
            encoder,
            0,
            m_global_bind_group,
            0,
            nullptr);

        wgpuRenderPassEncoderSetBindGroup(
            encoder,
            1,
            draw_call.bind_group,
            0,
            nullptr);
        wgpuRenderPassEncoderDrawIndexed(
            encoder,
            draw_call.end - draw_call.begin,
            1,
            draw_call.begin,
            0,
            0);

        m_frame->end_pass(encoder);
    }

    m_draw_calls.clear();
}
