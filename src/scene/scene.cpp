#include "noworry/scene/scene.h"

Scene::Scene(Renderer& renderer, Camera& camera)
    : m_camera(&camera)
{
    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(CameraData);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(renderer.device(), &buffer_desc);

    m_bind_group = renderer
        .mesh_vertex_shader().create_global_group(renderer.device(), m_buffer);
}

Scene::Scene(Scene&& other)
{
    m_buffer = nullptr;
    m_bind_group = nullptr;
    m_camera = nullptr;

    *this = std::move(other);
}

Scene& Scene::operator=(Scene&& other)
{
    std::swap(m_uniforms, other.m_uniforms);
    std::swap(m_buffer, other.m_buffer);
    std::swap(m_bind_group, other.m_bind_group);
    std::swap(m_camera, other.m_camera);

    return *this;
}

Scene::~Scene()
{
    if (m_bind_group != nullptr) {
        wgpuBindGroupRelease(m_bind_group);
    }
    if (m_buffer != nullptr) {
        wgpuBufferRelease(m_buffer);
    }
}

void Scene::set_camera(Camera& camera)
{
    m_camera = &camera;
}

void Scene::render(Frame& frame)
{
    m_camera->update_matrix(&m_uniforms.camera);
    WGPUQueue queue = wgpuDeviceGetQueue(frame.renderer().device());
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_uniforms, sizeof(GlobalUniforms));

    for (auto& ptr : children()) {
        ptr->render(frame);
    }
}
