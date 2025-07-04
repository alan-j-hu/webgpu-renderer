#ifndef SPRITE_PIPELINE_H
#define SPRITE_PIPELINE_H

#include <webgpu/webgpu.h>

class SpritePipeline
{
public:
    SpritePipeline(WGPUDevice device);
    SpritePipeline(const SpritePipeline&) = delete;
    SpritePipeline(SpritePipeline&&);

    SpritePipeline& operator=(const SpritePipeline&) = delete;
    SpritePipeline& operator=(SpritePipeline&& other);

    virtual ~SpritePipeline();

    WGPURenderPipeline pipeline() { return m_pipeline; }

    WGPUBindGroup
    create_bind_group(WGPUDevice, WGPUTextureView, WGPUSampler) const;

private:
    WGPUBindGroupLayout m_bg_layout;
    WGPUPipelineLayout m_pipeline_layout;
    WGPURenderPipeline m_pipeline;

    void cleanup();
    void move(SpritePipeline&&);
};

#endif
