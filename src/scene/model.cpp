#include "noworry/material/material.h"
#include "noworry/scene/model.h"
#include "noworry/mesh.h"
#include <cstring>
#include <glm/ext/matrix_transform.hpp>

Model::Model(
    WGPUDevice device, MeshEffect& effect, const Mesh& mesh, Material& mat)
    : m_mesh(mesh)
{
    m_model.transform = glm::identity<glm::mat4>();

    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(Model);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(device, &buffer_desc);

    m_bind_group = effect.create_model_group(device,
        m_buffer, mesh.vertex_buffer, mesh.vertex_count);

    m_material = &mat;

    m_translation = glm::vec3(0.0f, 0.0f, 0.0f);
    m_scale = 1;
}

Model::~Model()
{
    wgpuBindGroupRelease(m_bind_group);
    wgpuBufferRelease(m_buffer);
}

void Model::set_translation(const glm::vec3& translation)
{
    m_translation = translation;
}

void Model::set_yaw(float yaw)
{
    m_yaw = yaw;
}

void Model::set_scale(float scale)
{
    m_scale = scale;
}

void Model::copy_to_gpu(WGPUDevice device)
{
    update_matrix();
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_model.transform, sizeof(float[4][4]));
    m_material->pipeline().enqueue(*this);
}

void Model::update_matrix()
{
    glm::mat4 transform = glm::mat4(1);
    // Unintuitively, the matrix argument is the *left* mtrix (i.e. the
    // the *next* step in the transformation. Therefore, the code is
    // written in the opposite order than the transformations.
    transform = glm::translate(transform, m_translation);
    transform = glm::rotate(transform, m_yaw, glm::vec3(0, 1, 0));
    m_model.transform = transform;
}
