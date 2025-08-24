#ifndef MATERIAL_H
#define MATERIAL_H

#include "Effect.h"
#include <webgpu/webgpu.h>

class Material
{
public:
    virtual ~Material() = default;

    virtual WGPUBindGroup bind_group() const = 0;
    virtual Effect& effect() const = 0;
};

#endif
