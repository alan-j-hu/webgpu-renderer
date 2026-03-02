#ifndef TILESET_THUMBNAILS_H
#define TILESET_THUMBNAILS_H

#include "TileThumbnail.h"
#include "../../Project.h"
#include "noworry/Gfx3D/ModelInstance.h"
#include "noworry/Gfx3D/RenderContext.h"
#include <memory>
#include <vector>

class AppState;

class TilesetThumbnails : public Tileset::Listener
{
public:
    TilesetThumbnails(AppState&, const Tileset&);
    virtual ~TilesetThumbnails();

    virtual void tile_added(int idx) override;
    virtual void tile_removed(TileDef&, int idx) override;
    virtual void tile_replaced(int idx) override;

    std::size_t count() const;

    TileThumbnail& at(int idx);
    const TileThumbnail& at(int idx) const;

    void capture(const TileDef& tiledef, RenderTarget& target);

private:
    AppState* m_app_state;
    const Tileset* m_tileset;
    RenderContext m_render_ctx;
    std::vector<std::unique_ptr<TileThumbnail>> m_thumbnails;
    ModelInstance m_instance;
};

#endif
