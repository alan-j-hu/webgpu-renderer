#include "TileMesh.h"
#include "../AppState.h"

#include <utility>

TileMesh::TileMesh(
    AppState& app_state,
    std::vector<Vertex> vertices,
    const std::vector<std::uint16_t>& indices)
    : m_vertices(std::move(vertices))
{
    m_mesh = std::make_unique<BasicMesh>(
        app_state.renderer().device(),
        WGPUPrimitiveTopology_TriangleList,
        m_vertices,
        indices,
        (indices.size() / 3) * 3);
}
