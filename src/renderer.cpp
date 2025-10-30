#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/transform.h"
#include "noworry/Material/FlatEffect.h"
#include "noworry/Material/TextureEffect.h"
#include "noworry/scene/scene.h"
#include <utility>

Frame::Frame(Renderer& renderer)
{
    m_renderer = &renderer;
}

Frame& Frame::add(const Mesh& mesh,
                  const Material& material,
                  const glm::mat4& transform)
{
    if (mesh.vertex_count() != 0 && mesh.index_count() != 0) {
        m_renderer->batcher().enqueue_parts(mesh, material, transform);
    }
    return *this;
}

Renderer::Renderer(WGPUDevice device)
    : m_device(device),
      m_mesh_vertex_shader(device),
      m_batcher(*this),
      m_object_group_pool(*this)
{
    m_effects.emplace_back(std::make_unique<FlatEffect>(m_device));
    m_effects.emplace_back(std::make_unique<TextureEffect>(m_device));

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

void Renderer::render(RenderTarget& target, Scene& scene, Camera& camera)
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
    do_render(scene, camera, pass);
    wgpuRenderPassEncoderEnd(pass);

    WGPUCommandBufferDescriptor buffer_desc = { 0 };
    WGPUCommandBuffer command_buffer =
        wgpuCommandEncoderFinish(encoder, &buffer_desc);
    wgpuQueueSubmit(queue, 1, &command_buffer);

    wgpuRenderPassEncoderRelease(pass);
    wgpuCommandEncoderRelease(encoder);
    wgpuCommandBufferRelease(command_buffer);

    m_object_group_pool.reset();
}

void Renderer::do_render(
    Scene& scene,
    Camera& camera,
    WGPURenderPassEncoder encoder)
{
    Frame frame(*this);
    scene.render(frame, camera);
    m_batcher.draw(encoder);
}
