#include "noworry/scene/scene.h"
#include "noworry/material/texturemesheffect.h"

Scene::Scene(Renderer& renderer)
    : m_renderer(&renderer)
{
    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(CameraData);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(renderer.device(), &buffer_desc);

    m_bind_group = renderer
        .uniform_layout().create_bind_group(renderer.device(), m_buffer);

    m_cameras.push_back(std::make_unique<Camera>());
}

Scene::Scene(Scene&& other)
    : m_renderer(other.m_renderer),
      m_camera(other.m_camera),
      m_uniforms(other.m_uniforms),
      m_buffer(other.m_buffer),
      m_bind_group(other.m_bind_group),
      m_cameras(std::move(other.m_cameras)),
      m_renderobjects(std::move(other.m_renderobjects))
{
    other.m_moved = true;
    other.m_buffer = nullptr;
    other.m_bind_group = nullptr;
}

Scene& Scene::operator=(Scene&& other)
{
    wgpuBindGroupRelease(m_bind_group);
    wgpuBufferRelease(m_buffer);

    m_renderer = other.m_renderer;
    m_camera = other.m_camera;
    m_uniforms = other.m_uniforms;
    m_buffer = other.m_buffer;
    m_bind_group = other.m_bind_group;
    m_cameras = std::move(other.m_cameras);
    m_renderobjects = std::move(other.m_renderobjects);

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

RenderObject& Scene::add_renderobject(const Mesh& mesh, Material& mat)
{
    m_renderobjects.push_back(
        std::make_unique<RenderObject>(
            m_renderer->device(),
            *m_renderer->mesh_effect<TextureMeshEffect>(),
            mesh,
            mat));
    return *m_renderobjects[m_renderobjects.size() - 1];
}

void Scene::update()
{
    Camera& camera = current_camera();
    camera.update_matrix(&m_uniforms.camera);
    WGPUQueue queue = wgpuDeviceGetQueue(m_renderer->device());
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_uniforms, sizeof(Uniforms));
}
