#ifndef TILESET_THUMBNAILS_H
#define TILESET_THUMBNAILS_H

#include "TileThumbnail.h"
#include "../../Project.h"
#include <memory>
#include <vector>

class AppState;

class TilesetThumbnails
{
public:
    TilesetThumbnails(AppState&, const Tileset&);
    virtual ~TilesetThumbnails();

    std::size_t count() const;

    TileThumbnail& at(int idx);
    const TileThumbnail& at(int idx) const;

private:
    class Listener : public Tileset::Listener
    {
    public:
        Listener(TilesetThumbnails&);

        void tile_added(int idx) override;
        void tile_removed(TileDef&, int idx) override;
        void tile_replaced(int idx) override;

    private:
        TilesetThumbnails* m_thumbnails;
    } m_listener;

    AppState* m_app_state;
    const Tileset* m_tileset;
    std::vector<std::unique_ptr<TileThumbnail>> m_thumbnails;
};

#endif
