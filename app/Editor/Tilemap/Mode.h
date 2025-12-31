#ifndef MODE_H
#define MODE_H

#include "noworry/rendertarget.h"
#include "noworry/Draw2D/SpriteRenderer.h"

class Mode
{
public:
    virtual void handle_input() = 0;

    virtual void draw_overlay(RenderTarget&, SpriteRenderer&) = 0;

    virtual void draw_controls() = 0;
};

#endif
