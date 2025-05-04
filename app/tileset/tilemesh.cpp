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
    m_model = &m_scene.add_model(mesh, editor.default_material());

    Camera& camera = m_scene.current_camera();
    camera.set_position(glm::vec3(1.0f, 2.0f, -0.5f));
    camera.set_target(glm::vec3(1.0f, 1.0f, 0.5f));
}
