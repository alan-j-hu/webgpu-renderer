#ifndef MODEL_H
#define MODEL_H

#include "pipeline.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct Mesh;

class Model
{
public:
    Model(WGPUDevice device, Pipeline& pipeline, const Mesh& mesh);
    virtual ~Model();

    const Mesh& mesh() const { return m_mesh; }
    WGPUBindGroup bind_group() { return m_bind_group; }

    void set_translation(const glm::vec3& translation);
    void set_yaw(float yaw);
    void set_scale(float scale);

    void copy_to_gpu(WGPUDevice device);

private:
    ModelData m_model;
    const Mesh& m_mesh;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;
    glm::vec3 m_translation;
    float m_yaw;
    float m_scale;

    void update_matrix();
};

#endif
