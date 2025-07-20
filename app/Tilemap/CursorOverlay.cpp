#include "CursorOverlay.h"
#include "TilemapEditor.h"

CursorOverlay::CursorOverlay(AppState& app_state, TilemapEditor& editor)
    : m_app_state(&app_state), m_editor(&editor)
{
}

void CursorOverlay::draw(RenderTarget& target, SpriteRenderer& sprite_renderer)
{
    const Project& project = m_app_state->project();
    TileThumbnail* thumb = nullptr;
    if (m_editor->selected_tile() != -1) {
        thumb = &m_app_state->tile_thumbnail(m_editor->selected_tile());
    }

    std::optional<std::pair<int, int>> cell_opt = m_editor->mouseover_cell();
    if (cell_opt) {
        glm::vec3 pos_3d(cell_opt->first, cell_opt->second, 0);
        // Top-left in 2D space
        glm::vec2 pos_tl =
            m_editor->map_3d_to_2d(pos_3d + glm::vec3(0, 1, 0));
        // Bottom-right in 2D space
        glm::vec2 pos_br =
            m_editor->map_3d_to_2d(pos_3d + glm::vec3(1, 0, 0));
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
