#ifndef CAMERA_H
#define CAMERA_H

#include "pipeline.h"

class Camera
{
public:
    Camera(WGPUDevice device, Pipeline& pipeline);
    virtual ~Camera();

    void copy_to_gpu(WGPUDevice device);

    CameraData m_camera;
    WGPUBindGroup m_bind_group;
    WGPUBuffer m_buffer;
};

#endif
