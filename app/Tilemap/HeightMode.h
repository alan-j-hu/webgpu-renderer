#ifndef HEIGHT_MODE_H
#define HEIGHT_MODE_H

#include "View2DMode.h"
#include "../AppState.h"

class TilemapEditor;

class HeightMode : public View2DMode
{
public:
    HeightMode(AppState&, TilemapEditor&);

    virtual void handle_input() override;

    virtual void draw_overlay(RenderTarget&, SpriteRenderer&) override;

    virtual void draw_controls() override;

private:
};

#endif
