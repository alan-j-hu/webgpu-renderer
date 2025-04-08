#include "noworry/flatmeshpipeline.h"
#include "noworry/mesh.h"

FlatMeshPipeline::FlatMeshPipeline(
    WGPUDevice device, FlatMeshEffect& effect)
    : m_device(device), m_effect(effect)
{
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
    fragment.module = m_effect.fragment_shader();
    fragment.entryPoint = {"fs_main", WGPU_STRLEN};
    fragment.targetCount = 1;
    fragment.targets = &color_target;

    WGPURenderPipelineDescriptor pipeline_desc = { 0 };
    pipeline_desc.label = {"pipeline", WGPU_STRLEN};
    pipeline_desc.layout = m_effect.pipeline_layout();
    pipeline_desc.vertex.module = m_effect.vertex_shader();
    pipeline_desc.vertex.entryPoint = {"vs_main", WGPU_STRLEN};
    pipeline_desc.vertex.bufferCount = 0;
    pipeline_desc.vertex.buffers = nullptr;
    pipeline_desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;
    pipeline_desc.primitive.cullMode = WGPUCullMode_None;
    pipeline_desc.depthStencil = &depth_stencil;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.mask = ~0u;
    pipeline_desc.fragment = &fragment;

    m_pipeline = wgpuDeviceCreateRenderPipeline(device, &pipeline_desc);
}

FlatMeshPipeline::~FlatMeshPipeline()
{
    wgpuRenderPipelineRelease(m_pipeline);
}

void FlatMeshPipeline::enqueue(Model& model)
{
    m_queue.push_back(&model);
}

void FlatMeshPipeline::draw(WGPURenderPassEncoder encoder)
{
    wgpuRenderPassEncoderSetPipeline(encoder, m_pipeline);
    for (auto model : m_queue) {
        int count = 3 * model->mesh().tri_count;
        wgpuRenderPassEncoderSetBindGroup(
            encoder, 1, model->material().bind_group(), 0, nullptr);
        wgpuRenderPassEncoderSetBindGroup(
            encoder, 2, model->bind_group(), 0, nullptr);

        wgpuRenderPassEncoderSetIndexBuffer(
            encoder,
            model->mesh().index_buffer,
            WGPUIndexFormat_Uint16,
            0,
            count * sizeof(std::uint16_t));

        wgpuRenderPassEncoderDrawIndexed(encoder, count, 1, 0, 0, 0);
    }
    m_queue.clear();
}
