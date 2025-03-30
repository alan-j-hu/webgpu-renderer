#ifndef PIPELINE_H
#define PIPELINE_H

#include "noworry/effect.h"
#include <glm/mat4x4.hpp>
#include <webgpu/webgpu.h>

class Pipeline
{
public:
    Pipeline(WGPUDevice device, Effect& effect);
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    ~Pipeline();

    WGPURenderPipeline pipeline() { return m_pipeline; }

private:
    WGPUDevice m_device;
    Effect& m_effect;
    WGPURenderPipeline m_pipeline;
};

#endif
