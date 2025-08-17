#include "noworry/Material/Material.h"
#include "noworry/Gfx3D/Mesh.h"
#include "noworry/scene/renderobject.h"
#include "noworry/renderer.h"

RenderObject::RenderObject(
    Renderer& renderer,
    Transform& transform,
    const Mesh& mesh,
    Material& material)
    : m_mesh(&mesh), m_material(&material)
{
    m_transform = transform;
}

void RenderObject::render(Frame& frame)
{
    frame.renderer().batcher().enqueue(*this);
}
