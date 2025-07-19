#ifndef CURSOR_OVERLAY_H
#define CURSOR_OVERLAY_H

#include "../AppState.h"
#include "noworry/rendertarget.h"
#include "noworry/Draw2D/SpriteRenderer.h"

class TilemapEditor;

class CursorOverlay
{
public:
    CursorOverlay(AppState&, TilemapEditor&);

    void draw(RenderTarget&, SpriteRenderer&);

private:
    AppState* m_app_state;
    TilemapEditor* m_editor;
};

#endif
