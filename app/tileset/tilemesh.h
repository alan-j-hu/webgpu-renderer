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
    TileMesh(
        TilesetEditor&,
        std::string,
        std::vector<Vertex>,
        std::vector<std::uint16_t>);

    const std::string& name() const { return m_name; }
    const std::vector<std::uint16_t>& indices() const { return m_indices; }

    void render_scene(Renderer&, RenderTarget&);

private:
    std::string m_name;
    std::unique_ptr<Mesh> m_mesh;
    std::vector<Vertex> m_vertices;
    std::vector<std::uint16_t> m_indices;

    TilesetEditor* m_tileset_editor;
    Scene m_scene;
    std::unique_ptr<RenderObject> m_model;
    std::unique_ptr<RenderObject> m_grid;
};

#endif
