#ifndef TILESET_TILEMESH_H
#define TILESET_TILEMESH_H

#include "noworry/mesh.h"
#include "noworry/scene/scene.h"
#include <cstdint>
#include <string>

class TilesetEditor;

class TileMesh
{
public:
    TileMesh(
        TilesetEditor&,
        std::string,
        Mesh&,
        std::vector<Vertex>,
        std::vector<std::uint16_t>);

    const std::string& name() const { return m_name; }
    Mesh& mesh() { return *m_mesh; }
    const std::vector<std::uint16_t>& indices() const { return m_indices; }
    Scene& scene() { return m_scene; }

private:
    std::string m_name;
    Mesh* m_mesh;
    std::vector<Vertex> m_vertices;
    std::vector<std::uint16_t> m_indices;

    TilesetEditor* m_tileset_editor;
    Scene m_scene;
    RenderObject* m_model;
    RenderObject* m_grid;
};

#endif
