#include "noworry/renderer.h"
#include <utility>

Renderer::Renderer(WGPUDevice device, int width, int height)
    : m_clear_color {0, 0, 0, 1},
      m_device(device),
      m_width(width),
      m_height(height),
      m_effect(device),
      m_pipeline(device, m_effect),
      m_camera(device, m_effect)
{
    create_depth_buffer(width, height);

    WGPUSamplerDescriptor sampler_desc = { 0 };
    sampler_desc.addressModeU = WGPUAddressMode_ClampToEdge;
    sampler_desc.addressModeV = WGPUAddressMode_ClampToEdge;
    sampler_desc.addressModeW = WGPUAddressMode_ClampToEdge;
    sampler_desc.magFilter = WGPUFilterMode_Nearest;
    sampler_desc.minFilter = WGPUFilterMode_Nearest;
    sampler_desc.mipmapFilter = WGPUMipmapFilterMode_Nearest;
    sampler_desc.lodMinClamp = 0;
    sampler_desc.lodMaxClamp = 1;
    sampler_desc.compare = WGPUCompareFunction_Undefined;
    sampler_desc.maxAnisotropy = 1;
    m_sampler = wgpuDeviceCreateSampler(m_device, &sampler_desc);
}

Renderer::~Renderer()
{
    wgpuSamplerRelease(m_sampler);
    wgpuTextureViewRelease(m_depth_texture_view);
    wgpuTextureRelease(m_depth_texture);
}

void Renderer::resize(int width, int height)
{
    wgpuTextureViewRelease(m_depth_texture_view);
    wgpuTextureRelease(m_depth_texture);
    create_depth_buffer(width, height);
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

    WGPURenderPassColorAttachment color_attachment = { 0 };
    color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    color_attachment.loadOp = WGPULoadOp_Clear;
    color_attachment.storeOp = WGPUStoreOp_Store;
    color_attachment.clearValue = m_clear_color;
    color_attachment.view = view;

    WGPURenderPassDepthStencilAttachment depth_stencil_attachment = { 0 };
    depth_stencil_attachment.view = m_depth_texture_view;
    depth_stencil_attachment.depthClearValue = 1.0f;
    depth_stencil_attachment.depthLoadOp = WGPULoadOp_Clear;
    depth_stencil_attachment.depthStoreOp = WGPUStoreOp_Store;
    depth_stencil_attachment.depthReadOnly = false;
    depth_stencil_attachment.stencilClearValue = 0.0f;
    depth_stencil_attachment.stencilLoadOp = WGPULoadOp_Undefined;
    depth_stencil_attachment.stencilStoreOp = WGPUStoreOp_Undefined;
    depth_stencil_attachment.stencilReadOnly = true;

    WGPURenderPassDescriptor render_pass_desc = { 0 };
    render_pass_desc.colorAttachmentCount = 1;
    render_pass_desc.colorAttachments = &color_attachment;
    render_pass_desc.depthStencilAttachment = &depth_stencil_attachment;

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

void Renderer::set_clear_color(WGPUColor color)
{
    m_clear_color = color;
}

void Renderer::create_depth_buffer(int width, int height)
{
    m_width = width;
    m_height = height;

    WGPUTextureDescriptor texture_desc = { 0 };
    texture_desc.dimension = WGPUTextureDimension_2D;
    texture_desc.format = WGPUTextureFormat_Depth24Plus;
    texture_desc.mipLevelCount = 1;
    texture_desc.sampleCount = 1;
    texture_desc.size.width = width;
    texture_desc.size.height = height;
    texture_desc.size.depthOrArrayLayers = 1;
    texture_desc.usage = WGPUTextureUsage_RenderAttachment;
    texture_desc.viewFormatCount = 0;
    texture_desc.viewFormats = nullptr;
    m_depth_texture = wgpuDeviceCreateTexture(m_device, &texture_desc);

    WGPUTextureViewDescriptor view_desc = { 0 };
    view_desc.aspect = WGPUTextureAspect_DepthOnly;
    view_desc.baseArrayLayer = 0;
    view_desc.arrayLayerCount = 1;
    view_desc.baseMipLevel = 0;
    view_desc.mipLevelCount = 1;
    view_desc.dimension = WGPUTextureViewDimension_2D;
    view_desc.format = WGPUTextureFormat_Depth24Plus;
    m_depth_texture_view = wgpuTextureCreateView(m_depth_texture, &view_desc);
}

void Renderer::do_render(WGPURenderPassEncoder encoder)
{
    for (auto& model : m_models) {
        model->material().pipeline().enqueue(*model);
    }
    wgpuRenderPassEncoderSetBindGroup(
        encoder, 0, m_camera.bind_group(), 0, nullptr);
    m_pipeline.draw(encoder, m_camera);
}
