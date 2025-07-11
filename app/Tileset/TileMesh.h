#ifndef TILESET_TILEMESH_H
#define TILESET_TILEMESH_H

#include "noworry/mesh.h"
#include "noworry/resourcetable.h"
#include "noworry/scene/scene.h"
#include <cstdint>
#include <memory>
#include <string>

class AppState;

/// Contains the GPU mesh for a tile shape, and a CPU copy of the vertices.
class TileMesh
{
public:
    TileMesh(AppState&,
             std::vector<Vertex>,
             const std::vector<std::uint16_t>&);
    TileMesh(TileMesh&&) = default;
    TileMesh& operator=(TileMesh&&) = default;

    const Mesh& mesh() const { return *m_mesh; }

    const std::vector<Vertex>& vertices() const { return m_vertices; }

private:
    std::unique_ptr<Mesh> m_mesh;
    std::vector<Vertex> m_vertices;
};

#endif
