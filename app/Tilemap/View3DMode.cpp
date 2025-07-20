#include "View3DMode.h"

View3DMode::View3DMode(AppState& app_state, TilemapEditor& editor)
    : m_app_state(&app_state),
      m_editor(&editor)
{
}

void View3DMode::handle_input()
{
}

void View3DMode::draw_overlay(
    RenderTarget& target,
    SpriteRenderer& sprite_renderer)
{
}

void View3DMode::draw_controls()
{
}
