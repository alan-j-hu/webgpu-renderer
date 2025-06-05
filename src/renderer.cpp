#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/transform.h"
#include "noworry/material/texturemesheffect.h"
#include "noworry/material/wireframemesheffect.h"
#include "noworry/scene/scene.h"
#include <utility>

Frame::Frame(Renderer& renderer, RenderTarget& target, Scene& scene)
{
    m_renderer = &renderer;
    m_target = &target;
    m_scene = &scene;
}

Frame& Frame::add(Transform& transform, const Mesh& mesh, Material& material)
{
    material.effect().enqueue(transform, mesh, material);
    return *this;
}

Frame::~Frame()
{
    m_scene->update(*m_renderer);
    m_renderer->render(*m_target, *m_scene);
}

Renderer::Renderer(WGPUDevice device)
    : m_device(device),
      m_uniform_layout(device)
{
    m_mesh_effects.emplace_back(
        std::make_unique<FlatMeshEffect>(device, m_uniform_layout));
    m_mesh_effects.emplace_back(
        std::make_unique<TextureMeshEffect>(device, m_uniform_layout));
    m_mesh_effects.emplace_back(
        std::make_unique<WireframeMeshEffect>(device, m_uniform_layout));

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
}

void Renderer::render(RenderTarget& target, Scene& scene)
{
    WGPUQueue queue = wgpuDeviceGetQueue(m_device);

    WGPUCommandEncoderDescriptor encoder_desc = { 0 };
    WGPUCommandEncoder encoder =
        wgpuDeviceCreateCommandEncoder(m_device, &encoder_desc);

    WGPURenderPassColorAttachment color_attachment = { 0 };
    color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    color_attachment.loadOp = WGPULoadOp_Clear;
    color_attachment.storeOp = WGPUStoreOp_Store;
    color_attachment.clearValue = target.clear_color();
    color_attachment.view = target.texture().view();

    WGPURenderPassDepthStencilAttachment depth_stencil_attachment = { 0 };
    depth_stencil_attachment.view = target.depth_texture().view();
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
    wgpuRenderPassEncoderSetBindGroup(
        pass, 0, scene.bind_group(), 0, nullptr);
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

void Renderer::do_render(WGPURenderPassEncoder encoder)
{
    for (auto& effect : m_mesh_effects) {
        effect->draw(*this, encoder);
    }
}
