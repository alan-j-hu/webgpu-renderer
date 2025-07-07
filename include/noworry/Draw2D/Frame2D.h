#ifndef FRAME_2D_H
#define FRAME_2D_H

#include "SpritePipeline.h"
#include "../rendertarget.h"
#include <webgpu/webgpu.h>

class Frame2D
{
public:
    Frame2D(WGPUDevice device, RenderTarget& target);
    Frame2D(Frame2D&&);

    Frame2D& operator=(Frame2D&& other);

    ~Frame2D();

    RenderTarget& target() { return *m_target; }

    WGPURenderPassEncoder pass() { return m_pass; }

    void finish();

private:
    RenderTarget* m_target;
    WGPUQueue m_queue;
    WGPUCommandEncoder m_encoder;
    WGPURenderPassEncoder m_pass;
};

#endif
