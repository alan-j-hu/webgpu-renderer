#include "noworry/Material/Material.h"
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
    m_transform = transform;
}
