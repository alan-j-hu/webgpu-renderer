#ifndef PIPELINE_H
#define PIPELINE_H

#include "noworry/camera.h"
#include "noworry/effect.h"
#include "noworry/model.h"
#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>
#include <webgpu/webgpu.h>

class Pipeline
{
public:
    Pipeline(WGPUDevice device, Effect& effect);
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

    ~Pipeline();

    void draw(WGPURenderPassEncoder encoder,
              Camera& camera,
              std::vector<std::unique_ptr<Model>>& models);
private:
    WGPUDevice m_device;
    Effect& m_effect;
    WGPURenderPipeline m_pipeline;
};

#endif
