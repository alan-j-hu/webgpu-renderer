#include "noworry/Material/Material.h"
#include "noworry/Gfx3D/BasicMesh.h"
#include "noworry/scene/renderobject.h"
#include "noworry/renderer.h"

RenderObject::RenderObject(
    Renderer& renderer,
    Transform& transform,
    const BasicMesh& mesh,
    Material& material)
    : m_mesh(&mesh), m_material(&material)
{
    m_transform = transform;
}
