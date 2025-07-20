#ifndef VIEW_3D_MODE_H
#define VIEW_3D_MODE_H

#include "Mode.h"
#include "../AppState.h"

class TilemapEditor;

class View3DMode : public Mode
{
public:
    View3DMode(AppState&, TilemapEditor&);

    virtual void handle_input() override;

    virtual void draw_overlay(RenderTarget&, SpriteRenderer&) override;

    virtual void draw_controls() override;

private:
    AppState* m_app_state;
    TilemapEditor* m_editor;
};

#endif
