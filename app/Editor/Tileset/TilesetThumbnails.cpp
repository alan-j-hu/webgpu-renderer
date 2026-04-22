#include "TilesetThumbnails.h"

TilesetThumbnails::TilesetThumbnails(
    AppState& app_state,
    const Tileset& tileset)
    : m_app_state(&app_state),
      m_tileset(&tileset),
      m_render_ctx(app_state.renderer())
{
    for (int i = 0; i < tileset.count(); ++i) {
        auto thumbnail = std::make_unique<TileThumbnail>(*m_app_state);
        capture(*m_tileset->at(i), thumbnail->render_target());
        m_thumbnails.emplace_back(std::move(thumbnail));
    }
    m_tileset->listenable().add_listener(*this);
}

TilesetThumbnails::~TilesetThumbnails()
{
    m_tileset->listenable().remove_listener(*this);
}

void TilesetThumbnails::tile_added(int idx)
{
    auto thumbnail = std::make_unique<TileThumbnail>(*m_app_state);
    capture(*m_tileset->at(idx), thumbnail->render_target());
    m_thumbnails.insert(m_thumbnails.begin() + idx, std::move(thumbnail));
}

void TilesetThumbnails::tile_removed(TileDef&, int idx)
{
    m_thumbnails.erase(m_thumbnails.begin() + idx);
}

void TilesetThumbnails::tile_replaced(int idx)
{
    capture(*m_tileset->at(idx), m_thumbnails.at(idx)->render_target());
}

std::size_t TilesetThumbnails::count() const
{
    return m_thumbnails.size();
}

TileThumbnail& TilesetThumbnails::at(int idx)
{
    return *m_thumbnails.at(idx);
}

const TileThumbnail& TilesetThumbnails::at(int idx) const
{
    return *m_thumbnails.at(idx);
}

void TilesetThumbnails::capture(const TileDef& tiledef, RenderTarget& target)
{
    OrthographicCamera camera;

    camera.set_top(5);
    camera.set_bottom(0);
    camera.set_left(0);
    camera.set_right(5);
    camera.set_position(glm::vec3(0, 0, 1));
    camera.set_target(glm::vec3(0, 0, 0));

    if (tiledef.model()) {
        m_instance.set_model(tiledef.model()->get());
    } else {
        m_instance.set_model(nullptr);
    }

    target.set_clear_color(m_app_state->background_color());
    m_app_state->renderer().render(
        m_render_ctx,
        m_instance,
        target,
        camera);
}
