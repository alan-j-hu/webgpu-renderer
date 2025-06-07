#include "tilemesh.h"
#include "../appstate.h"

#include <utility>

TileMesh::TileMesh(
    AppState& app_state,
    std::vector<Vertex> vertices,
    const std::vector<std::uint16_t>& indices)
    : m_vertices(std::move(vertices))
{
    m_mesh = std::make_unique<Mesh>(
        app_state.renderer().device(),
        m_vertices.data(), m_vertices.size(),
        indices.data(), indices.size(),
        (indices.size() / 3) * 3);
}
