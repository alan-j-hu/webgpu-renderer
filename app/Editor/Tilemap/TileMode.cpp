#include "TileMode.h"
#include "../Editor.h"

#include <numbers>
#include <imgui.h>

TileMode::TileMode(AppState& app_state, Editor& editor)
    : View2DMode(app_state, editor),
      m_tile_picker(app_state, editor),
      m_rotation(Rotation::Rotate0)
{
}

void TileMode::draw_overlay(
    RenderTarget& target,
    SpriteRenderer& sprite_renderer)
{
    const Project& project = app_state().project();
    auto& world = project.world_at(0);

    TileThumbnail* thumb = nullptr;
    const TileDef* tiledef = nullptr;

    auto tiledef_idx = app_state().selected_tiledef_idx();
    if (tiledef_idx) {

        thumb = &editor().tileset_thumbnails(0).at(*tiledef_idx);
        tiledef = world.tileset()->at(*tiledef_idx).get();
    }

    std::optional<std::pair<int, int>> cell_opt = editor().mouseover_cell();
    if (cell_opt && thumb != nullptr) {
        const float MAX_LENGTH = 5.0;

        Region src;
        src.x = 0;
        src.y = 1 - tiledef->depth() / MAX_LENGTH;
        src.w = tiledef->width() / MAX_LENGTH;
        src.h = tiledef->depth() / MAX_LENGTH;

        float radians = 0;
        int dest_x = cell_opt->first;
        int dest_y = cell_opt->second;
        switch (m_rotation) {
        case Rotation::Rotate90:
            radians = 0.5 * std::numbers::pi;
            dest_x += tiledef->depth();
            break;
        case Rotation::Rotate180:
            radians = std::numbers::pi;
            dest_x += tiledef->width();
            dest_y += tiledef->depth();
            break;
        case Rotation::Rotate270:
            radians = 1.5 * std::numbers::pi;
            dest_y += tiledef->width();
            break;
        }

        Region dest = region(
            dest_x,
            dest_y,
            tiledef->width(),
            tiledef->depth());

        sprite_renderer.draw(
            thumb->spritesheet(),
            glm::vec2(dest.x, dest.y),
            dest.w,
            dest.h,
            src,
            radians,
            glm::vec2(0, dest.h));
    }
}

void TileMode::draw_controls()
{
    if (ImGui::RadioButton("0°", m_rotation == Rotation::Rotate0)) {
        m_rotation = Rotation::Rotate0;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("90°", m_rotation == Rotation::Rotate90)) {
        m_rotation = Rotation::Rotate90;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("180°", m_rotation == Rotation::Rotate180)) {
        m_rotation = Rotation::Rotate180;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("270°", m_rotation == Rotation::Rotate270)) {
        m_rotation = Rotation::Rotate270;
    }

    auto idx = app_state().selected_tiledef_idx();
    m_tile_picker.render(idx);
    app_state().select_tiledef(idx);
}

void TileMode::handle_left_mouse_down(int x, int y)
{
    auto& project = app_state().project();
    auto selected_tiledef = app_state().selected_tiledef();
    Layer* selected_layer = app_state().selected_layer();

    if (selected_layer != nullptr && selected_tiledef) {
        Layer& layer = *selected_layer;
        short z = editor().z_palette().selected_z();

        if (m_command.get() == nullptr) {
            auto command = std::make_unique<PlaceTilesCommand>(
                layer, z, m_rotation,
                *selected_tiledef);
            app_state().push_command(std::move(command), &m_command);
        }

        m_command.get()->add_placement(x, y);
        app_state().update_current_command();
    }
}

void TileMode::handle_left_mouse_released(int x, int y)
{
    if (m_command.get() != nullptr) {
        app_state().finish_current_command();
    }
}

void TileMode::handle_right_mouse_down(int x, int y)
{
    auto& project = app_state().project();
    Layer* layer = app_state().selected_layer();

    if (layer != nullptr) {
        if (m_command.get() == nullptr) {
            auto command = std::make_unique<PlaceTilesCommand>(*layer);
            app_state().push_command(std::move(command), &m_command);
        }

        m_command.get()->add_placement(x, y);
        app_state().update_current_command();
    }
}

void TileMode::handle_right_mouse_released(int x, int y)
{
    if (m_command.get() != nullptr) {
        app_state().finish_current_command();
    }
}
