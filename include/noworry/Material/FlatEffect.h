#ifndef FLATEFFECT_H
#define FLATEFFECT_H

#include "Effect.h"

/// An effect that displays a flat color.
class FlatEffect : public Effect
{
public:
    FlatEffect(WGPUDevice device);
    FlatEffect(const FlatEffect&) = delete;
    FlatEffect& operator=(const FlatEffect&) = delete;
    virtual ~FlatEffect();

    virtual WGPUShaderModule fragment_shader() override
    {
        return m_fragment_shader;
    }

    virtual WGPUBindGroupLayout layout() override
    {
        return m_material_layout;
    }

    WGPUBindGroup create_material_group(WGPUDevice, WGPUBuffer);

private:
    WGPUShaderModule m_fragment_shader;
    WGPUBindGroupLayout m_material_layout;
};

#endif
