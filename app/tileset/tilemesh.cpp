#include "tilemesh.h"
#include "modelinspector.h"

#include <utility>

TileMesh::TileMesh(
    ModelInspector& editor,
    std::vector<Vertex> vertices,
    const std::vector<std::uint16_t>& indices)
    : m_vertices(std::move(vertices))
{
    m_mesh = std::make_unique<Mesh>(
        editor.renderer().device(),
        m_vertices.data(), m_vertices.size(),
        indices.data(), indices.size());

    m_model = std::make_unique<RenderObject>(
        editor.renderer().device(), *m_mesh, editor.default_material());
}
