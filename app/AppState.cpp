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
      m_thumbnail_util(*this)
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
        m_undo_stack.push_back(std::move(command));
        m_redo_stack.clear();
    }
}

void AppState::undo()
{
    if (m_undo_stack.size() == 0) {
        return;
    }
    auto command = std::move(m_undo_stack.back());
    m_undo_stack.pop_back();
    command->undo(m_project);
    m_redo_stack.push_back(std::move(command));
}

void AppState::redo()
{
    if (m_redo_stack.size() == 0) {
        return;
    }
    auto command = std::move(m_redo_stack.back());
    m_redo_stack.pop_back();
    command->redo(m_project);
    m_undo_stack.push_back(std::move(command));
}
