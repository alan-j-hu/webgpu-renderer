#include "noworry/Draw2D/SpriteBatch.h"
#include <cstdint>

SpriteBatch::SpriteBatch(WGPUDevice device, std::size_t max)
    : m_capacity(max),
      m_device(device),
      m_pipeline(device),
      m_is_drawing(false)
{
    const std::size_t ibuffer_size = 6 * sizeof(std::uint16_t) * max;
    std::vector<std::uint16_t> indices;
    indices.reserve(6 * max);
    for (int i = 0; i < max; ++i) {
        int index = 6 * i;
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
    vbuffer_desc.size = 4 * sizeof(glm::vec4) * max;
    vbuffer_desc.mappedAtCreation = false;
    m_vertex_buffer = wgpuDeviceCreateBuffer(device, &vbuffer_desc);

    WGPUBufferDescriptor ibuffer_desc = { 0 };
    ibuffer_desc.nextInChain = nullptr;
    ibuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
    ibuffer_desc.size = ibuffer_size;
    ibuffer_desc.mappedAtCreation = false;
    m_index_buffer = wgpuDeviceCreateBuffer(device, &ibuffer_desc);

    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(queue, m_index_buffer, 0,
                         indices.data(), ibuffer_size);
}

SpriteBatch::~SpriteBatch()
{
    if (m_index_buffer != nullptr) {
        wgpuBufferRelease(m_index_buffer);
    }
    if (m_vertex_buffer != nullptr) {
        wgpuBufferRelease(m_vertex_buffer);
    }
}

void SpriteBatch::begin(WGPURenderPassEncoder encoder)
{
    m_encoder = encoder;
    m_current_draw_call.bind_group = nullptr;
    m_current_draw_call.begin = 0;
    m_is_drawing = true;
}

void SpriteBatch::end()
{
    m_current_draw_call.end = 6 * (m_vertices.size() + 1);
    m_draw_calls.push_back(m_current_draw_call);
    flush();
    m_current_draw_call.bind_group = nullptr;
    m_current_draw_call.begin = 0;
    m_encoder = nullptr;
    m_is_drawing = false;
}

void SpriteBatch::draw(
    const Spritesheet& spritesheet,
    const Region& dest,
    const Region& src)
{
    if (!m_is_drawing) {
        return;
    }

    if (m_vertices.size() == 4 * m_capacity) {
        flush();
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

void SpriteBatch::flush()
{
    if (!m_is_drawing) {
        return;
    }
    if (m_draw_calls.size() == 0) {
        return;
    }

    WGPUQueue queue = wgpuDeviceGetQueue(m_device);
    wgpuQueueWriteBuffer(queue, m_vertex_buffer, 0, m_vertices.data(),
                         sizeof(glm::vec4) * m_vertices.size());

    m_vertices.clear();

    wgpuRenderPassEncoderSetPipeline(m_encoder, m_pipeline.pipeline());
    wgpuRenderPassEncoderSetVertexBuffer(
        m_encoder, 0, m_vertex_buffer, 0, wgpuBufferGetSize(m_vertex_buffer));
    wgpuRenderPassEncoderSetIndexBuffer(
        m_encoder, m_index_buffer, WGPUIndexFormat_Uint16, 0,
        wgpuBufferGetSize(m_index_buffer));

    for (int i = 0; i < m_draw_calls.size(); ++i) {
        DrawCall& draw_call = m_draw_calls[i];
        if (draw_call.begin == draw_call.end) {
            continue;
        }
        wgpuRenderPassEncoderSetBindGroup(
            m_encoder,
            0,
            draw_call.bind_group,
            0,
            nullptr);
        wgpuRenderPassEncoderDrawIndexed(
            m_encoder,
            draw_call.end - draw_call.begin,
            1,
            draw_call.begin,
            0,
            0);
    }

    m_draw_calls.clear();
}
