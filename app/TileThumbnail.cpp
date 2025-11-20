#include "TileThumbnail.h"
#include "AppState.h"
#include "noworry/Gfx3D/ModelInstance.h"

ThumbnailCapture::ThumbnailCapture(AppState& app_state)
    : m_app_state(&app_state),
      m_scene(m_app_state->renderer())
{
}

void ThumbnailCapture::capture(
    RenderTarget& target,
    Renderable& renderable)
{
    OrthographicCamera camera;

    camera.set_top(16);
    camera.set_bottom(0);
    camera.set_left(0);
    camera.set_right(16);
    camera.set_position(glm::vec3(0, 0, 1));
    camera.set_target(glm::vec3(0, 0, 0));

    m_scene.children().clear();
    m_scene
        .children()
        .push_back(std::make_unique<RenderableRef>(renderable));

    m_app_state->renderer().render(target, m_scene, camera);
}

void ThumbnailCapture::capture(
    RenderTarget& target,
    Camera& camera,
    Transform& transform,
    const Model& model)
{
    m_scene.children().clear();
    m_scene
        .children()
        .push_back(std::make_unique<ModelInstance>(model));

    m_app_state->renderer().render(target, m_scene, camera);
}

TileThumbnail::TileThumbnail(AppState& app_state)
    : m_render_target(app_state.renderer().device(), 256, 256),
      m_spritesheet(app_state.renderer().device(),
                    app_state.sprite_renderer().pipeline(),
                    m_render_target.texture(),
                    app_state.renderer().default_sampler())
{
}
