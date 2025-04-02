#ifndef FLATMESHPIPELINE_H
#define FLATMESHPIPELINE_H

#include "camera.h"
#include "flatmesheffect.h"
#include "pipeline.h"
#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>
#include <webgpu/webgpu.h>

class FlatMeshPipeline : public Pipeline
{
public:
    FlatMeshPipeline(WGPUDevice device, FlatMeshEffect& effect);
    FlatMeshPipeline(const FlatMeshPipeline&) = delete;
    FlatMeshPipeline& operator=(const FlatMeshPipeline&) = delete;

    ~FlatMeshPipeline();

    FlatMeshEffect& effect() { return m_effect; }

    void enqueue(Model& model);

    void draw(WGPURenderPassEncoder encoder, Camera& camera);
private:
    WGPUDevice m_device;
    FlatMeshEffect& m_effect;
    WGPURenderPipeline m_pipeline;

    std::vector<Model*> m_queue;
};

#endif
