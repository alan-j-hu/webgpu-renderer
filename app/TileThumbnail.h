#ifndef TILE_THUMBNAIL_H
#define TILE_THUMBNAIL_H

#include "noworry/rendertarget.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/Draw2D/Spritesheet.h"
#include "noworry/Gfx3D/Model.h"
#include "noworry/Gfx3D/Renderable.h"
#include "noworry/scene/scene.h"

struct AppState;

class ThumbnailCapture
{
public:
    ThumbnailCapture(AppState&);

    void capture(RenderTarget& target, Renderable&);

    void capture(
        RenderTarget& target,
        Camera& camera,
        Transform& transform,
        const Model& model);

private:
    AppState* m_app_state;
    Scene m_scene;
};

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
