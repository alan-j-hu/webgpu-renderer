#include "noworry/scene/scene.h"

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

Scene::~Scene()
{
    wgpuBindGroupRelease(m_bind_group);
    wgpuBufferRelease(m_buffer);
}

Model& Scene::add_model(const Mesh& mesh, Material& mat)
{
    m_models.push_back(
        std::make_unique<Model>(
            m_renderer->device(),
            m_renderer->texture_mesh_effect(),
            mesh,
            mat));
    return *m_models[m_models.size() - 1];
}

void Scene::update()
{
    Camera& camera = current_camera();
    camera.update_matrix(&m_uniforms.camera);
    WGPUQueue queue = wgpuDeviceGetQueue(m_renderer->device());
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_uniforms, sizeof(Uniforms));
}
