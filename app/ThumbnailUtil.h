#ifndef THUMBNAIL_UTIL_H
#define THUMBNAIL_UTIL_H

#include "noworry/rendertarget.h"
#include "noworry/camera/orthographiccamera.h"
#include "noworry/Draw2D/Spritesheet.h"
#include "noworry/Gfx3D/Model.h"
#include "noworry/Gfx3D/Renderable.h"
#include "noworry/scene/scene.h"

struct AppState;

class ThumbnailUtil
{
public:
    ThumbnailUtil(AppState&);

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

#endif
