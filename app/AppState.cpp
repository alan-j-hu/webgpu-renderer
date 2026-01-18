#include "AppState.h"
#include "noworry/grid.h"

#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

AppState::CurrentCommand::CurrentCommand()
    : m_command(nullptr), m_holder(nullptr)
{
}

AppState::CurrentCommand::CurrentCommand(
    std::unique_ptr<Command> command,
    CommandHolderBase* holder)
    : m_command(std::move(command)),
      m_holder(holder)
{
}

Command* AppState::CurrentCommand::get()
{
    return m_command.get();
}

std::unique_ptr<Command> AppState::CurrentCommand::clear()
{
    if (m_holder != nullptr) {
        m_holder->reset();
        m_holder = nullptr;
    }
    auto ptr = std::move(m_command);
    m_command = nullptr;
    return ptr;
}

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

void AppState::update_current_command()
{
    if (m_current_command.get() != nullptr) {
        if (auto* command = m_current_command.get()) {
            if (command->first_do(m_project) == Command::Outcome::DONE) {
                finish_current_command();
            }
        }
    }
}

void AppState::finish_current_command()
{
    if (m_current_command.get() != nullptr) {
        m_current_command.get()->finish();
        auto command = m_current_command.clear();
        m_undo_stack.push_back(std::move(command));
        m_redo_stack.clear();
    }
}

void AppState::undo()
{
    if (m_current_command.get() != nullptr) {
        m_current_command.get()->undo(m_project);
        auto ptr = m_current_command.clear();
        m_redo_stack.push_back(std::move(ptr));
    } else if (m_undo_stack.size() > 0) {
        auto command = std::move(m_undo_stack.back());
        m_undo_stack.pop_back();
        command->undo(m_project);
        m_redo_stack.push_back(std::move(command));
    }
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
