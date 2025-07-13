#ifndef CURSOR_OVERLAY_H
#define CURSOR_OVERLAY_H

#include "../AppState.h"
#include "noworry/rendertarget.h"
#include "noworry/Draw2D/SpriteBatch.h"

class TilemapEditor;

class CursorOverlay
{
public:
    CursorOverlay(AppState&, TilemapEditor&);

    void draw(RenderTarget&, SpriteBatch& sprite_batch);

private:
    AppState* m_app_state;
    TilemapEditor* m_editor;
};

#endif
