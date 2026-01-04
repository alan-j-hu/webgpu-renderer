#include "TilesetThumbnails.h"

TilesetThumbnails::Listener::Listener(TilesetThumbnails& thumbnails)
    : m_thumbnails(&thumbnails)
{
}

void TilesetThumbnails::Listener::tile_added(int idx)
{
    m_thumbnails->m_thumbnails.insert(
        m_thumbnails->m_thumbnails.begin() + idx,
        std::make_unique<TileThumbnail>(
            *m_thumbnails->m_app_state,
            *m_thumbnails->m_tileset->at(idx)));
}

void TilesetThumbnails::Listener::tile_removed(TileDef&, int idx)
{
    m_thumbnails->m_thumbnails.erase(
        m_thumbnails->m_thumbnails.begin() + idx);
}

void TilesetThumbnails::Listener::tile_replaced(int idx)
{
    m_thumbnails->m_thumbnails.at(idx) = std::make_unique<TileThumbnail>(
        *m_thumbnails->m_app_state,
        *m_thumbnails->m_tileset->at(idx));
}

TilesetThumbnails::TilesetThumbnails(
    AppState& app_state,
    const Tileset& tileset)
    : m_app_state(&app_state), m_tileset(&tileset), m_listener(*this)
{
    for (int i = 0; i < tileset.count(); ++i) {
        m_thumbnails.emplace_back(
            std::make_unique<TileThumbnail>(
                *m_app_state,
                *m_tileset->at(i)));
    }
    m_tileset->listenable().add_listener(m_listener);
}

TilesetThumbnails::~TilesetThumbnails()
{
    m_tileset->listenable().remove_listener(m_listener);
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

