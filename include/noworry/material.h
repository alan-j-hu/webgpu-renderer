#ifndef MATERIAL_H
#define MATERIAL_H

#include "pipeline.h"
#include <webgpu/webgpu.h>

class Material
{
public:
    virtual ~Material() = default;

    virtual WGPUBindGroup bind_group() = 0;
    virtual Pipeline& pipeline() = 0;
};

#endif
