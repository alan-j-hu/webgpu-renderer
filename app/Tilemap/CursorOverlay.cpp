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

    glm::vec2 mouse_pos = m_editor->mouse_pos();
    glm::vec3 pos_3d = m_editor->map_2d_to_3d(mouse_pos);
    glm::vec2 pos_bl = m_editor->map_3d_to_2d(pos_3d);
    glm::vec2 pos_tr = m_editor->map_3d_to_2d(pos_3d + glm::vec3(1, 1, 0));

    if (thumb != nullptr) {
        Region src;
        src.x = 0;
        src.y = 0;
        src.w = 1;
        src.h = 1;

        Region dest;
        dest.x = pos_bl.x;
        dest.y = pos_bl.y;
        dest.w = pos_tr.x - pos_bl.x;
        dest.h = pos_bl.y - pos_tr.y;

        sprite_renderer.draw(thumb->spritesheet(),
                          dest,
                          src);
    }
}
