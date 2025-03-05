#ifndef MODEL_H
#define MODEL_H

#include "pipeline.h"

class ModelBinding
{
public:
    ModelBinding(WGPUDevice device, Pipeline pipeline);

    void copy_to_gpu(WGPUDevice device);

    Model m_model;
    WGPUBindGroup m_bind_group;
    WGPUBuffer m_buffer;
};

#endif
