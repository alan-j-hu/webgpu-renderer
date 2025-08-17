#include "noworry/Gfx3D/RenderBatch.h"
#include "noworry/Gfx3D/BasicMesh.h"
#include "noworry/renderer.h"
#include "noworry/Material/Material.h"

DrawCall::DrawCall(const Mesh& p_mesh,
                   Material& p_material,
                   const Transform& p_transform)
    : mesh(&p_mesh),
      material_group(p_material.bind_group()),
      transform(p_transform)
{
}

void RenderBatch::set_pipeline(Pipeline& pipeline)
{
    m_pipeline = &pipeline;
}

void RenderBatch::enqueue(RenderObject& object)
{
    m_draw_calls.emplace_back(
        object.mesh(), object.material(), object.transform());
}

void RenderBatch::enqueue_parts(const Mesh& mesh,
                                Material& mat,
                                const Transform& transform)
{
    m_draw_calls.emplace_back(mesh, mat, transform);
}

void RenderBatch::draw(Renderer& renderer, WGPURenderPassEncoder encoder)
{
    wgpuRenderPassEncoderSetPipeline(encoder, m_pipeline->pipeline());
    for (auto& draw_call : m_draw_calls) {
        int count = draw_call.mesh->index_count();

        ObjectBindGroup* group = renderer.bind_group_pool().alloc();
        group->copy(renderer, draw_call.transform);

        wgpuRenderPassEncoderSetBindGroup(
            encoder, 1, draw_call.material_group, 0, nullptr);
        wgpuRenderPassEncoderSetBindGroup(
            encoder, 2, group->bind_group(), 0, nullptr);

        wgpuRenderPassEncoderSetVertexBuffer(
            encoder,
            0,
            draw_call.mesh->vertex_buffer(),
            0,
            wgpuBufferGetSize(draw_call.mesh->vertex_buffer()));

        wgpuRenderPassEncoderSetIndexBuffer(
            encoder,
            draw_call.mesh->index_buffer(),
            WGPUIndexFormat_Uint16,
            0,
            count * sizeof(std::uint16_t));

        wgpuRenderPassEncoderDrawIndexed(encoder, count, 1, 0, 0, 0);
    }

    m_draw_calls.clear();
}

RenderBatcher::RenderBatcher(Renderer& renderer)
{
    m_pool_index = 0;
    m_renderer = &renderer;
}

void RenderBatcher::enqueue(RenderObject& object)
{
    RenderBatch* batch = search(object.material(), object.mesh());
    batch->enqueue(object);
}

void RenderBatcher::enqueue_parts(const Mesh& mesh,
                                  Material& mat,
                                  const Transform& transform)
{
    RenderBatch* batch = search(mat, mesh);
    batch->enqueue_parts(mesh, mat, transform);
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
        m_pool.push_back(std::make_unique<RenderBatch>());
    }
    RenderBatch* batch = m_pool[m_pool_index++].get();
    batch->set_pipeline(pipeline);
    return batch;
}

void RenderBatcher::reset()
{
    m_pool_index = 0;
    m_batches.clear();
}

RenderBatch* RenderBatcher::search(Material& material, const Mesh& mesh)
{
    PipelineKey key = {};

    key.effect = &material.effect();
    key.topology = mesh.topology();

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
