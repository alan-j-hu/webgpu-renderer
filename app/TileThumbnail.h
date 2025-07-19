#ifndef TILE_THUMBNAIL_H
#define TILE_THUMBNAIL_H

#include "noworry/rendertarget.h"
#include "noworry/Draw2D/Spritesheet.h"

struct AppState;

class TileThumbnail
{
public:
    TileThumbnail(AppState&);

    RenderTarget& render_target() { return m_render_target; }
    Spritesheet& spritesheet() { return m_spritesheet; }
    WGPUTextureView texture_view() { return m_render_target.texture().view(); }

private:
    RenderTarget m_render_target;
    Spritesheet m_spritesheet;
};

#endif
