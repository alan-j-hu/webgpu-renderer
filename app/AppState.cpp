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
      m_thumbnail_util(*this),
      m_selected_tiledef_idx(),
      m_selected_level_idx(0, 0),
      m_selected_layer_idx()
{
    m_background_color = {0.0f, 0.5f, 0.5f, 1.0f};
    m_default_material = &m_resources.add_flat_material(0.5, 0.5, 0.5);
    m_wireframe_material = &m_resources.add_wireframe_material(1, 1, 1);
}

void AppState::set_project(Project project)
{
    m_project = std::move(project);
}

std::optional<int> AppState::selected_tiledef_idx() const
{
    return m_selected_tiledef_idx;
}

std::optional<std::shared_ptr<TileDef>> AppState::selected_tiledef()
{
    if (auto idx = m_selected_tiledef_idx) {
        return m_project.tileset_at(0)->at(*idx);
    }
    return std::nullopt;
}

void AppState::select_tiledef(std::optional<int> idx)
{
    m_selected_tiledef_idx = idx;
}

Level* AppState::selected_level()
{
    return m_project
        .world_at(0)
        .level_at(m_selected_level_idx.x, m_selected_level_idx.y);
}

void AppState::select_level(glm::ivec2 idx)
{
    m_selected_level_idx = idx;
}

std::optional<int> AppState::selected_layer_idx() const
{
    return m_selected_layer_idx;
}

Layer* AppState::selected_layer()
{
    if (!m_selected_layer_idx) {
        return nullptr;
    }

    auto level = selected_level();
    if (level == nullptr) {
        return nullptr;
    }
    return &level->layer_at(*m_selected_layer_idx);
}

void AppState::select_layer(std::optional<int> idx)
{
    m_selected_layer_idx = idx;
}

void AppState::update_current_command()
{
    if (m_current_command.get() != nullptr) {
        if (auto* command = m_current_command.get()) {
            auto expected = command->first_do(m_project);
            if (expected && *expected == Command::Outcome::DONE) {
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
        m_undo_stack.push_back({
            m_selected_tiledef_idx,
            m_selected_level_idx,
            m_selected_layer_idx,
            std::move(command)
        });
        m_redo_stack.clear();
    }
}

void AppState::undo()
{
    if (m_current_command.get() != nullptr) {
        m_current_command.get()->undo(m_project);
        auto command = m_current_command.clear();
        m_redo_stack.push_back({
            m_selected_tiledef_idx,
            m_selected_level_idx,
            m_selected_layer_idx,
            std::move(command)
        });
    } else if (m_undo_stack.size() > 0) {
        auto snapshot = std::move(m_undo_stack.back());
        m_undo_stack.pop_back();

        snapshot.command->undo(m_project);
        m_redo_stack.push_back({
            m_selected_tiledef_idx,
            m_selected_level_idx,
            m_selected_layer_idx,
            std::move(snapshot.command)
        });

        m_selected_tiledef_idx = snapshot.selected_tiledef_idx;
        m_selected_level_idx = snapshot.selected_level_idx;
        m_selected_layer_idx = snapshot.selected_layer_idx;
    }
}

void AppState::redo()
{
    if (m_redo_stack.size() == 0) {
        return;
    }
    auto snapshot = std::move(m_redo_stack.back());
    m_redo_stack.pop_back();

    snapshot.command->redo(m_project);
    m_undo_stack.push_back({
        m_selected_tiledef_idx,
        m_selected_level_idx,
        m_selected_layer_idx,
        std::move(snapshot.command)
    });

    m_selected_tiledef_idx = snapshot.selected_tiledef_idx;
    m_selected_level_idx = snapshot.selected_level_idx;
    m_selected_layer_idx = snapshot.selected_layer_idx;
}
