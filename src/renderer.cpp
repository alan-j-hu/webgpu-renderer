#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/transform.h"
#include "noworry/Material/FlatEffect.h"
#include "noworry/Material/TextureEffect.h"
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
    RenderObject ro(*m_renderer, transform, mesh, material);
    m_renderer->pipeline_factory().enqueue(*m_renderer, ro);
    //material.effect().enqueue(*m_renderer, transform, mesh, material);
    return *this;
}

Frame::~Frame()
{
    m_scene->update(*m_renderer);
    m_renderer->render(*m_target, *m_scene);
}

ModelGroup::ModelGroup(Renderer& renderer)
{
    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(ModelUniforms);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(renderer.device(), &buffer_desc);

    m_bind_group =
        renderer.mesh_vertex_shader().create_model_group(renderer.device(),
                                                         m_buffer);
}

ModelGroup::ModelGroup(ModelGroup&& other)
{
    m_buffer = other.m_buffer;
    m_bind_group = other.m_bind_group;
    other.m_buffer = nullptr;
    other.m_bind_group = nullptr;
}

ModelGroup& ModelGroup::operator=(ModelGroup&& other)
{
    if (m_buffer != nullptr) {
        wgpuBufferRelease(m_buffer);
    }
    if (m_bind_group != nullptr) {
        wgpuBindGroupRelease(m_bind_group);
    }

    m_buffer = other.m_buffer;
    m_bind_group = other.m_bind_group;
    other.m_buffer = nullptr;
    other.m_bind_group = nullptr;

    return *this;
}

ModelGroup::~ModelGroup()
{
    if (m_buffer != nullptr) {
        wgpuBufferRelease(m_buffer);
    }
    if (m_bind_group != nullptr) {
        wgpuBindGroupRelease(m_bind_group);
    }
}

void ModelGroup::copy(Renderer& renderer, Transform& transform)
{
    ModelUniforms uniforms {};
    transform.update_matrix(uniforms);

    WGPUQueue queue = wgpuDeviceGetQueue(renderer.device());
    wgpuQueueWriteBuffer(queue, m_buffer, 0, &uniforms, sizeof(float[16]));
}

Renderer::Renderer(WGPUDevice device)
    : m_device(device),
      m_mesh_vertex_shader(device),
      m_next_group(0)
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

ModelGroup& Renderer::alloc_group()
{
    while (m_next_group >= m_model_groups.size()) {
        m_model_groups.push_back(std::make_unique<ModelGroup>(*this));
    }
    return *m_model_groups[m_next_group++];
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

    m_next_group = 0;
}

void Renderer::do_render(WGPURenderPassEncoder encoder)
{
    m_pipeline_factory.draw(encoder);
}
