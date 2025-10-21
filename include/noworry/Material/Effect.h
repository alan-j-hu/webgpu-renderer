#ifndef MATERIAL_EFFECT_H
#define MATERIAL_EFFECT_H

#include <webgpu/webgpu.h>

/// An effect specifies a fragment shader and its material parameters.
class Effect
{
public:
    virtual ~Effect() = default;

    virtual WGPUShaderModule fragment_shader() = 0;
    virtual WGPUBindGroupLayout layout() = 0;
};

#endif
