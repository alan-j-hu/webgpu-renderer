#ifndef TILE_MODE_H
#define TILE_MODE_H

#include "View2DMode.h"
#include "../../AppState.h"
#include "../../TilePicker.h"
#include "noworry/rendertarget.h"
#include "noworry/Draw2D/SpriteRenderer.h"

class Editor;

class TileMode : public View2DMode
{
public:
    TileMode(AppState&, Editor&);

    virtual void draw_overlay(RenderTarget&, SpriteRenderer&) override;

    virtual void draw_controls() override;

protected:
    virtual void handle_click(int tile_x, int tile_y) override;

private:
    TilePicker m_tile_picker;
    int m_selected_tile;
    Rotation m_rotation;
};

#endif
