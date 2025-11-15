#ifndef VIEW_2D_MODE_H
#define VIEW_2D_MODE_H

#include "Mode.h"

class AppState;
class TilemapEditor;

class View2DMode : public Mode
{
public:
    View2DMode(AppState&, TilemapEditor&);

    virtual void handle_input() override;

protected:
    AppState& app_state();

    TilemapEditor& editor();

    virtual void handle_click(int tile_x, int tile_y);

private:
    AppState* m_app_state;
    TilemapEditor* m_editor;
};

#endif
