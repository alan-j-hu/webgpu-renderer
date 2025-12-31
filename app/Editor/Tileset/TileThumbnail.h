#ifndef TILE_THUMBNAIL_H
#define TILE_THUMBNAIL_H

#include "../../AppState.h"
#include "../../Project.h"
#include <memory>

class TileThumbnail
{
public:
    TileThumbnail(AppState&, const TileDef&);

    RenderTarget& render_target() { return m_render_target; }
    Spritesheet& spritesheet() { return *m_spritesheet; }
    WGPUTextureView texture_view() { return m_render_target.texture().view(); }

    void capture(const TileDef&);

private:
    AppState* m_app_state;
    RenderTarget m_render_target;
    std::unique_ptr<Spritesheet> m_spritesheet;
};

#endif
