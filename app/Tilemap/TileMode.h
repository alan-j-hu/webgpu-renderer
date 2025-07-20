#ifndef TILE_MODE_H
#define TILE_MODE_H

#include "Mode.h"
#include "../AppState.h"
#include "../TilePicker.h"
#include "noworry/rendertarget.h"
#include "noworry/Draw2D/SpriteRenderer.h"

class TilemapEditor;

class TileMode : public Mode
{
public:
    TileMode(AppState&, TilemapEditor&);

    virtual void handle_input() override;

    virtual void draw_overlay(RenderTarget&, SpriteRenderer&) override;

    virtual void draw_controls() override;

private:
    AppState* m_app_state;
    TilemapEditor* m_editor;
    TilePicker m_tile_picker;
    int m_selected_tile;
};

#endif
