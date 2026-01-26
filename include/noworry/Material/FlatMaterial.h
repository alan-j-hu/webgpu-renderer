#ifndef FLATMATERIAL_H
#define FLATMATERIAL_H

#include "Material.h"
#include "FlatEffect.h"
#include <webgpu/webgpu.h>

class FlatMaterial : public Material
{
public:
    FlatMaterial(
        int id,
        WGPUDevice device,
        FlatEffect&,
        float r, float g, float b);

    int id() const;
    virtual WGPUBindGroup bind_group() const override;
    virtual FlatEffect& effect() const override;
    virtual glm::vec4 diffuse_factor() const override;

private:
    glm::vec4 m_color;
    int m_id;
    FlatEffect* m_effect;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;
};

#endif
