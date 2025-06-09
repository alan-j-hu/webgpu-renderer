#include "noworry/material/material.h"
#include "noworry/scene/renderobject.h"
#include "noworry/mesh.h"
#include "noworry/renderer.h"

RenderObject::RenderObject(
    Renderer& renderer,
    Transform& transform,
    const Mesh& mesh,
    Material& material)
    : m_mesh(&mesh), m_material(&material)
{
    m_model_group = &renderer.alloc_group();
    m_model_group->copy(renderer, transform);
}

WGPUBindGroup RenderObject::bind_group()
{
    return m_model_group->bind_group();
}
