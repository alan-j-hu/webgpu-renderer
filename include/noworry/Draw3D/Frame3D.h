#ifndef FRAME_3D_H
#define FRAME_3D_H

#include <webgpu/webgpu.h>

class RenderTarget;

class Frame3D
{
public:
    Frame3D(WGPUDevice device, RenderTarget&);
    Frame3D(Frame3D&&);
    Frame3D& operator=(Frame3D&&);
    ~Frame3D();

    WGPURenderPassEncoder pass() { return m_pass; }

    void finish();
private:
    WGPUQueue m_queue;
    WGPUCommandEncoder m_encoder;
    WGPURenderPassEncoder m_pass;
};

#endif
