#include "TileThumbnail.h"
#include "AppState.h"

TileThumbnail::TileThumbnail(AppState& app_state)
    : m_render_target(app_state.renderer().device(), 64, 64),
      m_spritesheet(app_state.renderer().device(),
                    app_state.sprite_renderer().pipeline(),
                    m_render_target.texture(),
                    app_state.renderer().default_sampler())
{
}
