#ifndef FLATMATERIAL_H
#define FLATMATERIAL_H

#include "material.h"
#include "flatmeshpipeline.h"
#include <webgpu/webgpu.h>

class FlatMaterial : public Material
{
public:
    FlatMaterial(
        int id,
        WGPUDevice device,
        FlatMeshPipeline&,
        float r, float g, float b);

    int id() { return m_id; }
    virtual WGPUBindGroup bind_group() override { return m_bind_group; }
    virtual FlatMeshPipeline& pipeline() override { return *m_pipeline; }
private:
    int m_id;
    FlatMeshPipeline* m_pipeline;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;
};

#endif
