#include "TileMode.h"
#include "TilemapEditor.h"
#include "../Commands/PlaceTileCommand.h"

#include "imgui.h"

TileMode::TileMode(AppState& app_state, TilemapEditor& editor)
    : View2DMode(app_state, editor),
      m_selected_tile(-1),
      m_tile_picker(app_state)
{
}

void TileMode::handle_input()
{
    auto& project = app_state().project();

    if (ImGui::IsMouseDown(0)) {
        auto cell_opt = editor().mouseover_cell();
        if (cell_opt) {
            int x = cell_opt->first;
            int y = cell_opt->second;

            int selected_layer = editor().selected_layer();

            if (selected_layer != -1 && m_selected_tile != -1) {
                short z = editor().z_palette().selected_z();

                app_state().push_command(std::make_unique<PlaceTileCommand>(
                    selected_layer, x, y, z,
                    project.tiledef_at(m_selected_tile)
                ));
            }
        }
    }
}

void TileMode::draw_overlay(
    RenderTarget& target,
    SpriteRenderer& sprite_renderer)
{
    const Project& project = app_state().project();
    TileThumbnail* thumb = nullptr;
    if (m_selected_tile != -1) {
        thumb = &app_state().tile_thumbnail(m_selected_tile);
    }

    std::optional<std::pair<int, int>> cell_opt = editor().mouseover_cell();
    if (cell_opt) {
        glm::vec3 pos_3d(cell_opt->first, cell_opt->second, 0);
        // Top-left in 2D space
        glm::vec2 pos_tl =
            editor().map_3d_to_2d(pos_3d + glm::vec3(0, 1, 0));
        // Bottom-right in 2D space
        glm::vec2 pos_br =
            editor().map_3d_to_2d(pos_3d + glm::vec3(1, 0, 0));
        // Beware, +1 goes towards the top of the screen in 3D, but towards
        // the bottom in 2D

        if (thumb != nullptr) {
            Region src;
            src.x = 0;
            src.y = 0;
            src.w = 1;
            src.h = 1;

            Region dest;
            dest.x = pos_tl.x;
            dest.y = pos_tl.y;
            dest.w = pos_br.x - pos_tl.x;
            dest.h = pos_br.y - pos_tl.y;

            sprite_renderer.draw(thumb->spritesheet(),
                                 dest,
                                 src);
        }
    }
}

void TileMode::draw_controls()
{
    m_tile_picker.render(m_selected_tile);
}
