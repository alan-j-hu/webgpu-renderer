#ifndef RENDERER_2D_H
#define RENDERER_2D_H

#include "SpritePipeline.h"
#include "../rendertarget.h"
#include <webgpu/webgpu.h>

class Frame2D
{
public:
    Frame2D(WGPUDevice device, RenderTarget& target);
    Frame2D(Frame2D&&);

    Frame2D operator=(Frame2D&& other);

    ~Frame2D();

    WGPURenderPassEncoder pass() { return m_pass; }

    void finish();

private:
    WGPUQueue m_queue;
    WGPUCommandEncoder m_encoder;
    WGPURenderPassEncoder m_pass;
};

class Renderer2D
{
public:
    Renderer2D(WGPUDevice);

    Frame2D begin(RenderTarget& target);
    SpritePipeline& pipeline() { return m_pipeline; }

private:
    WGPUDevice m_device;
    SpritePipeline m_pipeline;
};

#endif
