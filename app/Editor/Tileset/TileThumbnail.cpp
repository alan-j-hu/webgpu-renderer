#include "TileThumbnail.h"

TileThumbnail::TileThumbnail(AppState& app_state)
    : m_app_state(&app_state),
      m_render_target(app_state.renderer().device(), 256, 256)
{
    m_spritesheet = std::make_unique<Spritesheet>(
        m_app_state->renderer().device(),
        m_app_state->sprite_renderer().pipeline(),
        m_render_target.texture(),
        m_app_state->renderer().default_sampler());
}
