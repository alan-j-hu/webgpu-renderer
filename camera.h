#ifndef CAMERA_H
#define CAMERA_H

#include "pipeline.h"

class CameraBinding
{
public:
    CameraBinding(WGPUDevice device, Pipeline pipeline);

    void copy_to_gpu(WGPUDevice device);

    Camera m_camera;
    WGPUBindGroup m_bind_group;
    WGPUBuffer m_buffer;
};

#endif
