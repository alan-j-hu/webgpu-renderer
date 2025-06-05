#include "noworry/transform.h"
#include "noworry/renderer.h"
#include <glm/ext/matrix_transform.hpp>

Transform::Transform(Renderer& renderer)
{
    m_model.transform = glm::identity<glm::mat4>();

    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(ModelUniforms);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(renderer.device(), &buffer_desc);

    m_bind_group =
        renderer.uniform_layout().create_model_group(renderer.device(),
                                                     m_buffer);

    m_translation = glm::vec3(0.0f, 0.0f, 0.0f);
    m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

Transform::~Transform()
{
    wgpuBindGroupRelease(m_bind_group);
    wgpuBufferRelease(m_buffer);
}

WGPUBindGroup Transform::bind_group(Renderer& renderer)
{
    update_matrix(renderer);
    return m_bind_group;
}

void Transform::update_matrix(Renderer& renderer)
{
    glm::mat4 transform = glm::mat4(1);
    // Unintuitively, the matrix argument is the *left* mtrix (i.e. the
    // the *next* step in the transformation. Therefore, the code is
    // written in the opposite order than the transformations.
    transform = glm::translate(transform, m_translation);
    transform = glm::rotate(transform, m_yaw, glm::vec3(0, 1, 0));
    transform = glm::scale(transform, m_scale);
    m_model.transform = transform;

    WGPUQueue queue = wgpuDeviceGetQueue(renderer.device());
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_model.transform, sizeof(float[16]));
}
