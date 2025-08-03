#include "noworry/Gfx3D/RenderBatch.h"
#include "noworry/mesh.h"
#include "noworry/renderer.h"
#include "noworry/Material/Material.h"

void RenderBatch::set_pipeline(Pipeline& pipeline)
{
    m_pipeline = &pipeline;
}

void RenderBatch::enqueue(RenderObject& object)
{
    m_draw_calls.push_back(&object);
}

void RenderBatch::draw(Renderer& renderer, WGPURenderPassEncoder encoder)
{
    wgpuRenderPassEncoderSetPipeline(encoder, m_pipeline->pipeline());
    for (auto object : m_draw_calls) {
        int count = object->mesh().index_count();

        ObjectBindGroup* group = renderer.bind_group_pool().alloc();
        group->copy(renderer, object->transform());

        wgpuRenderPassEncoderSetBindGroup(
            encoder, 1, object->material().bind_group(), 0, nullptr);
        wgpuRenderPassEncoderSetBindGroup(
            encoder, 2, group->bind_group(), 0, nullptr);

        wgpuRenderPassEncoderSetVertexBuffer(
            encoder,
            0,
            object->mesh().vertex_buffer(),
            0,
            wgpuBufferGetSize(object->mesh().vertex_buffer()));

        wgpuRenderPassEncoderSetIndexBuffer(
            encoder,
            object->mesh().index_buffer(),
            WGPUIndexFormat_Uint16,
            0,
            count * sizeof(std::uint16_t));

        wgpuRenderPassEncoderDrawIndexed(encoder, count, 1, 0, 0, 0);
    }

    m_draw_calls.clear();
}

RenderBatcher::RenderBatcher(Renderer& renderer)
{
    m_renderer = &renderer;
}

void RenderBatcher::enqueue(RenderObject& object)
{
    RenderBatch* batch = search(object);
    batch->enqueue(object);
}

void RenderBatcher::draw(WGPURenderPassEncoder encoder)
{
    for (auto& pair : m_batches) {
        pair.second->draw(*m_renderer, encoder);
    }

    reset();
}

RenderBatch* RenderBatcher::next_free(Pipeline& pipeline)
{
    if (m_pool_index >= m_pool.size()) {
        m_pool.emplace_back();
    }
    RenderBatch& batch = m_pool[m_pool_index++];
    batch.set_pipeline(pipeline);
    return &batch;
}

void RenderBatcher::reset()
{
    m_pool_index = 0;
    m_batches.clear();
}

RenderBatch* RenderBatcher::search(RenderObject& object)
{
    PipelineKey key = {};

    key.effect = &object.material().effect();
    key.topology = object.mesh().topology();

    auto it = m_batches.find(key);
    if (it == m_batches.end()) {
        Pipeline& pipeline =
            m_renderer->pipeline_factory().get_pipeline(*m_renderer, key);
        RenderBatch* batch = next_free(pipeline);
        m_batches.emplace(key, batch);
        return batch;
    }

    return it->second;
}
