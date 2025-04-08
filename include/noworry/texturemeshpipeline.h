#ifndef TEXTUREMESHPIPELINE_H
#define TEXTUREMESHPIPELINE_H

#include "pipeline.h"
#include "texturemesheffect.h"
#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>
#include <webgpu/webgpu.h>

class TextureMeshPipeline : public Pipeline
{
public:
    TextureMeshPipeline(WGPUDevice device, TextureMeshEffect& effect);
    TextureMeshPipeline(const TextureMeshPipeline&) = delete;
    TextureMeshPipeline& operator=(const TextureMeshPipeline&) = delete;

    ~TextureMeshPipeline();

    TextureMeshEffect& effect() { return m_effect; }

    void enqueue(Model& model);

    void draw(WGPURenderPassEncoder encoder);
private:
    WGPUDevice m_device;
    TextureMeshEffect& m_effect;
    WGPURenderPipeline m_pipeline;

    std::vector<Model*> m_queue;
};

#endif
