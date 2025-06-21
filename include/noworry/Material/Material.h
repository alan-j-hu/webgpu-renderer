#ifndef MATERIAL_H
#define MATERIAL_H

#include "Effect.h"
#include <webgpu/webgpu.h>

class Material
{
public:
    virtual ~Material() = default;

    virtual WGPUBindGroup bind_group() = 0;
    virtual Effect& effect() = 0;
};

#endif
