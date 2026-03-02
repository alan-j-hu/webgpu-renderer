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
    reset_selection_state();
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

void AppState::select_level(int x, int y)
{
    m_selected_level_idx = glm::ivec2(x, y);
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

bool AppState::update_long_command()
{
    if (m_command_holder == nullptr) {
        return false;
    }
    if (!m_command_holder->needs_update()) {
        return false;
    }

    if (auto command = m_command_holder->get()) {
        m_command_holder->set_needs_update(false);
        auto expected = command->first_do(m_project);
        if (expected && *expected == Command::Outcome::DONE) {
            command->finish();
            m_command_holder->reset();
        }
    }

    return true;
}

void AppState::finish_long_command()
{
    if (m_command_holder == nullptr) {
        return;
    }
    if (auto command = m_command_holder->get()) {
        command->finish();
        m_command_holder->reset();
    }
}

void AppState::run_pending_commands()
{
    std::vector<std::unique_ptr<Command>> commands;
    std::swap(commands, m_command_queue);
    m_error.reset();

    bool changed = update_long_command();

    auto size = commands.size();
    for (int i = 0; i < size; ++i) {
        auto& command = commands.at(i);
        auto outcome = command->first_do(m_project);
        if (!outcome) {
            m_error = std::make_optional<std::string>(outcome.error());
            break;
        }

        if (*outcome != Command::Outcome::UNCHANGED) {
            changed = true;
            m_undo_stack.push_back({
                m_selected_tiledef_idx,
                m_selected_level_idx,
                m_selected_layer_idx,
                std::move(command)
            });
        }

        if (i != size - 1 && *outcome == Command::Outcome::IN_PROGRESS) {
            command->finish();
        }
    }

    if (changed) {
        m_redo_stack.clear();
        reset_selection_state();
    }
}

void AppState::undo()
{
    if (m_undo_stack.size() == 0) {
        return;
    }
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

std::optional<std::string> AppState::check_error()
{
    std::optional<std::string> optional;
    std::swap(optional, m_error);
    return optional;
}

void AppState::reset_selection_state()
{
    auto tileset = m_project.tileset_at(0);

    if (m_selected_tiledef_idx) {
        int idx = *m_selected_tiledef_idx;
        if (tileset->count() == 0) {
            m_selected_tiledef_idx = std::nullopt;
        } else if (idx >= tileset->count()) {
            m_selected_tiledef_idx = tileset->count() - 1;
        }
    }

    auto& world = m_project.world_at(0);
    const Level* level = world.level_at(
        m_selected_level_idx.x,
        m_selected_level_idx.y);

    if (world.begin() == world.end()) {
    } else if (level == nullptr) {
        level = world.begin()->second.get();
        m_selected_level_idx = glm::ivec2(level->x(), level->y());
    }

    if (level->layer_count() == 0) {
        m_selected_layer_idx = std::nullopt;
    } else if (*m_selected_layer_idx >= level->layer_count()) {
        m_selected_layer_idx = level->layer_count() - 1;
    }
}
