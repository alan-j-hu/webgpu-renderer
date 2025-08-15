#include "noworry/Pipeline/PipelineFactory.h"
#include "noworry/Gfx3D/BasicMesh.h"
#include "noworry/renderer.h"
#include "noworry/Material/Material.h"
#include <utility>

Pipeline::Pipeline(Renderer& renderer, const PipelineKey& key)
{
    m_queued = false;

    // Layout
    WGPUBindGroupLayout layouts[3] = {
        renderer.mesh_vertex_shader().global_layout(),
        key.effect->layout(),
        renderer.mesh_vertex_shader().model_layout()
    };

    WGPUPipelineLayoutDescriptor pipeline_layout_desc = { 0 };
    pipeline_layout_desc.bindGroupLayoutCount = 3;
    pipeline_layout_desc.bindGroupLayouts = layouts;

    m_pipeline_layout =
        wgpuDeviceCreatePipelineLayout(renderer.device(),
                                       &pipeline_layout_desc);

    // Pipeline
    WGPUVertexBufferLayout vertex_buffer_layout;
    vertex_buffer_layout.attributeCount = Vertex::attributes().size();
    vertex_buffer_layout.attributes = Vertex::attributes().data();
    vertex_buffer_layout.arrayStride = sizeof(Vertex);
    vertex_buffer_layout.stepMode = WGPUVertexStepMode_Vertex;

    WGPUDepthStencilState depth_stencil = { 0 };
    depth_stencil.format = WGPUTextureFormat_Depth24Plus;
    depth_stencil.depthWriteEnabled = WGPUOptionalBool_True;
    depth_stencil.depthCompare = WGPUCompareFunction_Less;

    WGPUBlendState blend = {
        .color= {
            .operation = WGPUBlendOperation_Add,
            .srcFactor = WGPUBlendFactor_Src,
            .dstFactor = WGPUBlendFactor_Zero
        },
        .alpha = {
            .operation = WGPUBlendOperation_Add,
            .srcFactor = WGPUBlendFactor_Src,
            .dstFactor = WGPUBlendFactor_Zero
        }
    };

    WGPUColorTargetState color_target = {
        .format = WGPUTextureFormat_BGRA8Unorm,
        .blend = &blend,
        .writeMask = WGPUColorWriteMask_All
    };

    WGPUFragmentState fragment = { 0 };
    fragment.module = key.effect->fragment_shader();
    fragment.entryPoint = {"fs_main", WGPU_STRLEN};
    fragment.targetCount = 1;
    fragment.targets = &color_target;

    WGPURenderPipelineDescriptor pipeline_desc = { 0 };
    pipeline_desc.label = {"pipeline", WGPU_STRLEN};
    pipeline_desc.layout = m_pipeline_layout;
    pipeline_desc.vertex.module =
        renderer.mesh_vertex_shader().vertex_shader();
    pipeline_desc.vertex.entryPoint = {"vs_main", WGPU_STRLEN};
    pipeline_desc.vertex.bufferCount = 1;
    pipeline_desc.vertex.buffers = &vertex_buffer_layout;
    pipeline_desc.primitive.topology = key.topology;
    pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;
    pipeline_desc.primitive.cullMode = WGPUCullMode_None;
    pipeline_desc.depthStencil = &depth_stencil;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.mask = ~0u;
    pipeline_desc.fragment = &fragment;

    m_pipeline =
        wgpuDeviceCreateRenderPipeline(renderer.device(), &pipeline_desc);
}

Pipeline::Pipeline(Pipeline&& other)
    : m_queued(other.m_queued),
      m_pipeline_layout(other.m_pipeline_layout),
      m_pipeline(other.m_pipeline)
{
    other.m_pipeline_layout = nullptr;
    other.m_pipeline = nullptr;
}

Pipeline& Pipeline::operator=(Pipeline&& other)
{
    if (m_pipeline != nullptr) {
        wgpuRenderPipelineRelease(m_pipeline);
    }
    if (m_pipeline_layout != nullptr) {
        wgpuPipelineLayoutRelease(m_pipeline_layout);
    }

    m_queued = other.m_queued;
    m_pipeline_layout = other.m_pipeline_layout;
    m_pipeline = other.m_pipeline;

    other.m_pipeline_layout = nullptr;
    other.m_pipeline = nullptr;

    return *this;
}

Pipeline::~Pipeline()
{
    if (m_pipeline != nullptr) {
        wgpuRenderPipelineRelease(m_pipeline);
    }
    if (m_pipeline_layout != nullptr) {
        wgpuPipelineLayoutRelease(m_pipeline_layout);
    }
}

void Pipeline::enqueue(RenderObject ro)
{
    m_render_queue.push_back(ro);
}

void Pipeline::draw(Renderer& renderer, WGPURenderPassEncoder encoder)
{
    wgpuRenderPassEncoderSetPipeline(encoder, m_pipeline);
    for (auto& render_object : m_render_queue) {
        int count = render_object.mesh().index_count();

        ObjectBindGroup* group = renderer.bind_group_pool().alloc();
        group->copy(renderer, render_object.transform());

        wgpuRenderPassEncoderSetBindGroup(
            encoder, 1, render_object.material().bind_group(), 0, nullptr);
        wgpuRenderPassEncoderSetBindGroup(
            encoder, 2, group->bind_group(), 0, nullptr);

        wgpuRenderPassEncoderSetVertexBuffer(
            encoder,
            0,
            render_object.mesh().vertex_buffer(),
            0,
            wgpuBufferGetSize(render_object.mesh().vertex_buffer()));

        wgpuRenderPassEncoderSetIndexBuffer(
            encoder,
            render_object.mesh().index_buffer(),
            WGPUIndexFormat_Uint16,
            0,
            count * sizeof(std::uint16_t));

        wgpuRenderPassEncoderDrawIndexed(encoder, count, 1, 0, 0, 0);
    }
    m_render_queue.clear();
}

Pipeline& PipelineFactory::get_pipeline(
    Renderer& renderer, const PipelineKey& key)
{
    auto it = m_pipelines.find(key);
    if (it != m_pipelines.end()) {
        return it->second;
    }

    Pipeline pipeline(renderer, key);
    auto pair = m_pipelines.emplace(key, std::move(pipeline));
    return pair.first->second;
}

void PipelineFactory::enqueue(Pipeline& pipeline)
{
    if (!pipeline.queued()) {
        m_pipeline_queue.push_back(&pipeline);
        pipeline.set_queued(true);
    }
}

void PipelineFactory::enqueue(Renderer& renderer, RenderObject ro)
{
    PipelineKey key {};
    key.effect = &ro.material().effect();
    key.topology = ro.mesh().topology();
    Pipeline& pipeline = get_pipeline(renderer, key);

    enqueue(pipeline);
    pipeline.enqueue(ro);
}

void PipelineFactory::draw(Renderer& renderer, WGPURenderPassEncoder encoder)
{
    for (Pipeline* pipeline : m_pipeline_queue) {
        pipeline->draw(renderer, encoder);
        pipeline->set_queued(false);
    }
    m_pipeline_queue.clear();
}
