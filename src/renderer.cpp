#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
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
    m_frame = Frame3D(m_device, target);

    wgpuRenderPassEncoderSetBindGroup(
        m_frame->pass(), 0, scene.bind_group(), 0, nullptr);
    do_render(m_frame->pass());

    m_frame->finish();
    m_frame = std::nullopt;

    m_next_group = 0;
}

void Renderer::do_render(WGPURenderPassEncoder encoder)
{
    m_pipeline_factory.draw(encoder);
}
