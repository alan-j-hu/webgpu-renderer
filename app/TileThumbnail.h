#ifndef TILE_THUMBNAIL_H
#define TILE_THUMBNAIL_H

#include "noworry/rendertarget.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/Draw2D/Spritesheet.h"
#include "noworry/scene/scene.h"

struct AppState;

class ThumbnailCapture
{
public:
    ThumbnailCapture(AppState&);

    void capture(
        RenderTarget& target,
        Transform& transform,
        Material& material,
        const Mesh& mesh);

private:
    AppState* m_app_state;
    Scene m_scene;
    OrthographicCamera m_camera;
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
