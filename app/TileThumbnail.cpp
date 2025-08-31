#include "TileThumbnail.h"
#include "AppState.h"
#include "noworry/Gfx3D/ModelInstance.h"

ThumbnailCapture::ThumbnailCapture(AppState& app_state)
    : m_app_state(&app_state),
      m_camera(),
      m_scene(m_app_state->renderer(), m_camera)
{
    m_camera.set_top(1);
    m_camera.set_bottom(0);
    m_camera.set_left(0);
    m_camera.set_right(1);
    m_camera.set_position(glm::vec3(0, 0, 1));
    m_camera.set_target(glm::vec3(0, 0, 0));
}

void ThumbnailCapture::capture(
    RenderTarget& target,
    Transform& transform,
    const Model& model)
{
    m_scene.children().clear();
    m_scene
        .children()
        .push_back(std::make_unique<ModelInstance>(model));

    m_app_state->renderer().render(target, m_scene);
}

TileThumbnail::TileThumbnail(AppState& app_state)
    : m_render_target(app_state.renderer().device(), 64, 64),
      m_spritesheet(app_state.renderer().device(),
                    app_state.sprite_renderer().pipeline(),
                    m_render_target.texture(),
                    app_state.renderer().default_sampler())
{
}
