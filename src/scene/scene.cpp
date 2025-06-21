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
    : m_uniforms(other.m_uniforms),
      m_buffer(other.m_buffer),
      m_bind_group(other.m_bind_group),
      m_camera(other.m_camera)
{
    other.m_moved = true;
    other.m_buffer = nullptr;
    other.m_bind_group = nullptr;
}

Scene& Scene::operator=(Scene&& other)
{
    wgpuBindGroupRelease(m_bind_group);
    wgpuBufferRelease(m_buffer);

    m_uniforms = other.m_uniforms;
    m_buffer = other.m_buffer;
    m_bind_group = other.m_bind_group;
    m_camera = other.m_camera;

    other.m_moved = true;
    other.m_buffer = nullptr;
    other.m_bind_group = nullptr;

    return *this;
}

Scene::~Scene()
{
    if (m_moved) return;
    wgpuBindGroupRelease(m_bind_group);
    wgpuBufferRelease(m_buffer);
}

void Scene::set_camera(Camera& camera)
{
    m_camera = &camera;
}

void Scene::update(Renderer& renderer)
{
    m_camera->update_matrix(&m_uniforms.camera);
    WGPUQueue queue = wgpuDeviceGetQueue(renderer.device());
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_uniforms, sizeof(GlobalUniforms));
}
