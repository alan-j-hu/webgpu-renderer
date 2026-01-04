#include "AppState.h"
#include "noworry/grid.h"

#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

AppState::AppState(WGPUDevice device)
    : m_renderer(device),
      m_sprite_renderer(device, 100),
      m_resources(m_renderer),
      m_deserializer(m_resources),
      m_small_grid_mesh(
          create_grid(device,
                      glm::vec3(0, 0, 0),
                      5,
                      5,
                      glm::vec3(5, 0, 0),
                      glm::vec3(0, 5, 0))),
      m_thumbnail_util(*this),
      m_history_cursor(0)
{
    m_background_color = {0.0f, 0.5f, 0.5f, 1.0f};
    m_default_material = &m_resources.add_flat_material(0.5, 0.5, 0.5);
    m_wireframe_material = &m_resources.add_wireframe_material(1, 1, 1);
}

void AppState::set_project(Project project)
{
    m_project = std::move(project);
}

void AppState::push_command(std::unique_ptr<Command> command)
{
    if (command->redo(m_project) == Command::Outcome::COMPLETED) {
        while (m_history.size() > m_history_cursor) {
            m_history.pop_back();
        }
        m_history.push_back(std::move(command));
        m_history_cursor = m_history.size();
    }
}

void AppState::undo()
{
    if (m_history_cursor <= 0 || m_history_cursor > m_history.size()) {
        return;
    }
    --m_history_cursor;
    m_history[m_history_cursor]->undo(m_project);
}

void AppState::redo()
{
    if (m_history_cursor < 0 || m_history_cursor >= m_history.size()) {
        return;
    }
    m_history[m_history_cursor]->redo(m_project);
    ++m_history_cursor;
}
