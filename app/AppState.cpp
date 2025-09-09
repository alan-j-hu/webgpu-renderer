#include "AppState.h"
#include "Tilemap/TilemapEditor.h"
#include "noworry/grid.h"

#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

AppState::AppState(WGPUDevice device)
    : m_renderer(device),
      m_sprite_renderer(device, 100),
      m_resources(m_renderer),
      m_small_grid_mesh(
          create_grid(device,
                      glm::vec3(0, 0, 0),
                      5,
                      5,
                      glm::vec3(5, 0, 0),
                      glm::vec3(0, 5, 0))),
      m_capture(*this)
{
    m_background_color = {0.0f, 0.5f, 0.5f, 1.0f};
    m_default_material = &m_resources.add_flat_material(0.5, 0.5, 0.5);
    m_wireframe_material = &m_resources.add_wireframe_material(1, 1, 1);
}

void AppState::push_command(std::unique_ptr<Command> command)
{
    command->redo(m_project);
    m_history.push_back(std::move(command));
}

void AppState::refresh_thumbnails()
{
    while (m_thumbnail_cache.size() < m_project.tiledef_count()) {
        m_thumbnail_cache.emplace_back(*this);
    }
    for (int i = 0; i < m_project.tiledef_count(); ++i) {
        RenderTarget& target = m_thumbnail_cache.at(i).render_target();
        auto def = m_project.tiledef_at(i);

        if (def->model) {
            Transform transform;
            m_capture.capture(target, transform, **def->model);
        }
    }
}

void AppState::connect_tilemap_editor(TilemapEditor& editor)
{
    m_project.add_layer_listenable().add_listener(editor.add_layer_listener());
}

void AppState::disconnect_tilemap_editor(TilemapEditor& editor)
{
    m_project
        .add_layer_listenable()
        .remove_listener(editor.add_layer_listener());
}
