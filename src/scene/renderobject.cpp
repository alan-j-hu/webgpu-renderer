#include "noworry/material/material.h"
#include "noworry/scene/renderobject.h"
#include "noworry/mesh.h"
#include <cstring>
#include <glm/ext/matrix_transform.hpp>

RenderObject::RenderObject(WGPUDevice device, const Mesh& mesh, Material& mat)
    : m_mesh(mesh)
{
    m_model.transform = glm::identity<glm::mat4>();

    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(ModelData);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(device, &buffer_desc);

    m_bind_group = mat.effect().create_model_group(device, m_buffer);

    m_material = &mat;

    m_translation = glm::vec3(0.0f, 0.0f, 0.0f);
    m_scale = 1;
}

RenderObject::~RenderObject()
{
    wgpuBindGroupRelease(m_bind_group);
    wgpuBufferRelease(m_buffer);
}

void RenderObject::set_material(Material& material)
{
    m_material = &material;
}

void RenderObject::set_translation(const glm::vec3& translation)
{
    m_translation = translation;
}

void RenderObject::set_yaw(float yaw)
{
    m_yaw = yaw;
}

void RenderObject::set_scale(float scale)
{
    m_scale = scale;
}

void RenderObject::enqueue(WGPUDevice device)
{
    update_matrix();
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_model.transform, sizeof(float[4][4]));
    m_material->effect().enqueue(*this);
}

void RenderObject::update_matrix()
{
    glm::mat4 transform = glm::mat4(1);
    // Unintuitively, the matrix argument is the *left* mtrix (i.e. the
    // the *next* step in the transformation. Therefore, the code is
    // written in the opposite order than the transformations.
    transform = glm::translate(transform, m_translation);
    transform = glm::rotate(transform, m_yaw, glm::vec3(0, 1, 0));
    m_model.transform = transform;
}
