#include "TileThumbnail.h"

TileThumbnail::TileThumbnail(AppState& app_state, const TileDef& tiledef)
    : m_app_state(&app_state),
      m_render_target(app_state.renderer().device(), 256, 256)
{
    capture(tiledef);
}

void TileThumbnail::capture(const TileDef& tiledef)
{
    OrthographicCamera camera;

    camera.set_top(5);
    camera.set_bottom(0);
    camera.set_left(0);
    camera.set_right(5);
    camera.set_position(glm::vec3(0, 0, 1));
    camera.set_target(glm::vec3(0, 0, 0));

    if (tiledef.model()) {
        Transform transform;
        m_app_state->thumbnail_util().capture(
            m_render_target,
            camera,
            transform,
            **tiledef.model());
    }

    m_spritesheet = std::make_unique<Spritesheet>(
        m_app_state->renderer().device(),
        m_app_state->sprite_renderer().pipeline(),
        m_render_target.texture(),
        m_app_state->renderer().default_sampler());
}
