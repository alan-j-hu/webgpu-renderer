#ifndef TILESET_TILEMESH_H
#define TILESET_TILEMESH_H

#include "noworry/mesh.h"
#include <cstdint>

class TileMesh
{
public:
    TileMesh(Mesh&, std::vector<Vertex>, std::vector<std::uint16_t>);

    Mesh& mesh() { return *m_mesh; }
    const std::vector<std::uint16_t>& indices() const { return m_indices; }

private:
    Mesh* m_mesh;
    std::vector<Vertex> m_vertices;
    std::vector<std::uint16_t> m_indices;
};

#endif
