#ifndef FLATMATERIAL_H
#define FLATMATERIAL_H

#include "material.h"
#include "flatmesheffect.h"
#include <webgpu/webgpu.h>

class FlatMaterial : public Material
{
public:
    FlatMaterial(
        int id,
        WGPUDevice device,
        FlatMeshEffect&,
        float r, float g, float b);

    int id() { return m_id; }
    virtual WGPUBindGroup bind_group() override { return m_bind_group; }
    virtual FlatMeshEffect& effect() override { return *m_effect; }

private:
    int m_id;
    FlatMeshEffect* m_effect;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;
};

#endif
