#ifndef TILESET_TILEMESH_H
#define TILESET_TILEMESH_H

#include "noworry/mesh.h"
#include "noworry/resourcetable.h"
#include "noworry/scene/scene.h"
#include <cstdint>
#include <memory>
#include <string>

class TilesetEditor;

class TileMesh
{
public:
    TileMesh(TilesetEditor&, std::vector<Vertex>, std::vector<std::uint16_t>);
    TileMesh(TileMesh&&) = default;
    TileMesh& operator=(TileMesh&&) = default;

    const std::vector<std::uint16_t>& indices() const { return m_indices; }

    const Mesh& mesh() const { return *m_mesh; }
    RenderObject& renderobject() { return *m_model; }

    TileMesh rotate90(TilesetEditor& editor, int x, int y);
    TileMesh rotate180(TilesetEditor& editor, int x, int y);
    TileMesh rotate270(TilesetEditor& editor, int x, int y);

private:
    std::unique_ptr<Mesh> m_mesh;
    std::vector<Vertex> m_vertices;
    std::vector<std::uint16_t> m_indices;

    TilesetEditor* m_tileset_editor;
    std::unique_ptr<RenderObject> m_model;

    TileMesh rotate(TilesetEditor&, int, int, int, int, int, int);
};

#endif
