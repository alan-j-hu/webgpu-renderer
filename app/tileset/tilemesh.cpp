#include "tilemesh.h"
#include <utility>

TileMesh::TileMesh(
    Mesh& mesh,
    std::vector<Vertex> vertices,
    std::vector<std::uint16_t> indices)
    : m_mesh(&mesh),
      m_vertices(std::move(vertices)),
      m_indices(std::move(indices))
{
}
