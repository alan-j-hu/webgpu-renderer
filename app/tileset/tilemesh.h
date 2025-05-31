#ifndef TILESET_TILEMESH_H
#define TILESET_TILEMESH_H

#include "noworry/mesh.h"
#include "noworry/resourcetable.h"
#include "noworry/scene/scene.h"
#include <cstdint>
#include <memory>
#include <string>

class ModelInspector;

class TileMesh
{
public:
    TileMesh(ModelInspector&,
             std::vector<Vertex>,
             const std::vector<std::uint16_t>&);
    TileMesh(TileMesh&&) = default;
    TileMesh& operator=(TileMesh&&) = default;

    const Mesh& mesh() const { return *m_mesh; }
    RenderObject& renderobject() { return *m_model; }

    const std::vector<Vertex>& vertices() const { return m_vertices; }

private:
    std::unique_ptr<Mesh> m_mesh;
    std::vector<Vertex> m_vertices;

    ModelInspector* m_tileset_editor;
    std::unique_ptr<RenderObject> m_model;
};

#endif
