#include "renderer.h"
#include <utility>

Renderer::Renderer(WGPUDevice device, int width, int height)
    : m_device(device),
      m_pipeline(device),
      m_camera(device, m_pipeline)
{
}

void Renderer::render(WGPUTextureView view)
{
    WGPUQueue queue = wgpuDeviceGetQueue(m_device);

    m_camera.copy_to_gpu(m_device);
    for (auto& model : m_models) {
        model->copy_to_gpu(m_device);
    }

    WGPUCommandEncoderDescriptor encoder_desc = { 0 };
    WGPUCommandEncoder encoder =
        wgpuDeviceCreateCommandEncoder(m_device, &encoder_desc);

    WGPURenderPassColorAttachment color_attachments = { 0 };
    color_attachments.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    color_attachments.loadOp = WGPULoadOp_Clear;
    color_attachments.storeOp = WGPUStoreOp_Store;
    color_attachments.clearValue = { 1, 1, 0, 1 };
    color_attachments.view = view;

    WGPURenderPassDescriptor render_pass_desc = {};
    render_pass_desc.colorAttachmentCount = 1;
    render_pass_desc.colorAttachments = &color_attachments;
    render_pass_desc.depthStencilAttachment = nullptr;

    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(
        encoder, &render_pass_desc);
    do_render(pass);
    wgpuRenderPassEncoderEnd(pass);

    WGPUCommandBufferDescriptor buffer_desc = { 0 };
    WGPUCommandBuffer command_buffer =
        wgpuCommandEncoderFinish(encoder, &buffer_desc);
    wgpuQueueSubmit(queue, 1, &command_buffer);

    wgpuRenderPassEncoderRelease(pass);
    wgpuCommandEncoderRelease(encoder);
    wgpuCommandBufferRelease(command_buffer);
}

Mesh& Renderer::add_mesh(Vertex* vertices, std::size_t count)
{
    m_meshes.push_back(
        std::make_unique<Mesh>(m_device, vertices, count));
    return *m_meshes[m_meshes.size() - 1];
}

Model& Renderer::add_model(const Mesh& mesh)
{
    m_models.push_back(
        std::make_unique<Model>(m_device, m_pipeline, mesh));
    return *m_models[m_models.size() - 1];
}

void Renderer::do_render(WGPURenderPassEncoder encoder)
{
    wgpuRenderPassEncoderSetPipeline(
        encoder, m_pipeline.pipeline());
    wgpuRenderPassEncoderSetBindGroup(
        encoder, 0, m_camera.m_bind_group, 0, nullptr);
    for (auto& model : m_models) {
        std::size_t count = 3 * model->m_mesh.tri_count;

        wgpuRenderPassEncoderSetBindGroup(
            encoder, 1, model->m_bind_group, 0, nullptr);
        wgpuRenderPassEncoderSetVertexBuffer(
            encoder, 0, model->m_mesh.vertex_buffer, 0,
            sizeof(Vertex) * count);
        wgpuRenderPassEncoderDraw(encoder, count, 1, 0, 0);
    }
}
