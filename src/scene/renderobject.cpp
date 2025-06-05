#include "noworry/material/material.h"
#include "noworry/scene/renderobject.h"
#include "noworry/mesh.h"
#include "noworry/renderer.h"

RenderObject::RenderObject(
    Transform& transform, const Mesh& mesh, Material& material)
    : m_transform(&transform), m_mesh(&mesh), m_material(&material)
{
}
