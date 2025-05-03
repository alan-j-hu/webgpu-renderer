#include "tiledefinition.h"
#include "tileseteditor.h"

TileDefinition::TileDefinition(
    TilesetEditor& tileset_editor, TileMesh& mesh, int w, int h)
    : m_width(w), m_height(h), m_mesh(&mesh),
      m_scene(tileset_editor.renderer()), m_tileset_editor(&tileset_editor)
{
    m_model = &m_scene.add_model(
        mesh.mesh(),
        tileset_editor.default_material());

    Camera& camera = m_scene.current_camera();
    camera.set_position(glm::vec3(0.5f, 2.0f, -2.0f));
    camera.set_target(glm::vec3(0.5f, 0.0f, 0.0f));
}
