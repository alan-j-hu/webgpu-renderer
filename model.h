#ifndef MODEL_H
#define MODEL_H

#include "pipeline.h"
#include "glm/mat4x4.hpp"

class Model
{
public:
    Model(WGPUDevice device, Pipeline& pipeline);
    virtual ~Model();

    void copy_to_gpu(WGPUDevice device);

    ModelData m_model;
    WGPUBindGroup m_bind_group;
    WGPUBuffer m_buffer;
};

#endif
