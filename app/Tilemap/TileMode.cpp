#include "TileMode.h"
#include "TilemapEditor.h"
#include "../Commands/PlaceTileCommand.h"
#include <imgui.h>
#include <numbers>

TileMode::TileMode(AppState& app_state, TilemapEditor& editor)
    : View2DMode(app_state, editor),
      m_selected_tile(-1),
      m_rotation(Rotation::Rotate0),
      m_tile_picker(app_state)
{
}

void TileMode::handle_click(int x, int y)
{
    auto& project = app_state().project();
    auto selected = editor().selected_layer();
    auto& world = project.world_at(selected.world);

    if (selected.layer != -1 && m_selected_tile != -1) {
        short z = editor().z_palette().selected_z();

        app_state().push_command(std::make_unique<PlaceTileCommand>(
            selected, x, y, z, m_rotation,
            world.tileset()->at(m_selected_tile)));
    }
}

void TileMode::draw_overlay(
    RenderTarget& target,
    SpriteRenderer& sprite_renderer)
{
    const Project& project = app_state().project();
    auto selected = editor().selected_layer();
    auto& world = project.world_at(selected.world);

    TileThumbnail* thumb = nullptr;
    const TileDef* tiledef = nullptr;

    if (m_selected_tile != -1) {
        thumb = &app_state().tile_thumbnail(m_selected_tile);
        tiledef = world.tileset()->at(m_selected_tile).get();
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

    m_tile_picker.render(m_selected_tile);
}
