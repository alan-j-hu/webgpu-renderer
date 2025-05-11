#include "tilemesh.h"
#include "tileseteditor.h"
#include <utility>

TileMesh::TileMesh(
    TilesetEditor& editor,
    std::string name,
    Mesh& mesh,
    std::vector<Vertex> vertices,
    std::vector<std::uint16_t> indices)
    : m_name(std::move(name)),
      m_mesh(&mesh),
      m_vertices(std::move(vertices)),
      m_indices(std::move(indices)),
      m_scene(editor.renderer())
{
    m_model = std::make_unique<RenderObject>(
        editor.renderer().device(), mesh, editor.default_material());
    m_grid = std::make_unique<RenderObject>(
        editor.renderer().device(),
        editor.grid_mesh(),
        editor.wireframe_material());

    Camera& camera = m_scene.camera();
    camera.set_position(glm::vec3(1.0f, 2.0f, -0.5f));
    camera.set_target(glm::vec3(1.0f, 1.0f, 0.5f));
}

void TileMesh::render_scene(Renderer& renderer, RenderTarget& target)
{
    Frame(renderer, target, m_scene)
        .add_renderobject(*m_model)
        .add_renderobject(*m_grid);
}
