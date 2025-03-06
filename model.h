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

    void set_translation(const glm::vec3& translation);
    void set_yaw(float yaw);
    void set_scale(float scale);

    void copy_to_gpu(WGPUDevice device);

    ModelData m_model;
    WGPUBindGroup m_bind_group;
    WGPUBuffer m_buffer;
    const Mesh& m_mesh;

    glm::vec3 m_translation;
    float m_yaw;
    float m_scale;

private:
    void update_matrix();
};

#endif
