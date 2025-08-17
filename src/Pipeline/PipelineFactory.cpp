#include "noworry/Pipeline/PipelineFactory.h"
#include "noworry/Gfx3D/BasicMesh.h"
#include "noworry/renderer.h"
#include "noworry/Material/Material.h"
#include <utility>

Pipeline::Pipeline(Renderer& renderer, const PipelineKey& key)
{
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
    : m_pipeline_layout(nullptr),
      m_pipeline(nullptr)
{
    *this = std::move(other);
}

Pipeline& Pipeline::operator=(Pipeline&& other)
{
    std::swap(m_pipeline_layout, other.m_pipeline_layout);
    std::swap(m_pipeline, other.m_pipeline);

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
