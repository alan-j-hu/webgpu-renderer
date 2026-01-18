#ifndef VIEW_2D_MODE_H
#define VIEW_2D_MODE_H

#include "Mode.h"

class AppState;
class Editor;

class View2DMode : public Mode
{
public:
    View2DMode(AppState&, Editor&);

    virtual void handle_input() override;

protected:
    AppState& app_state();

    Editor& editor();

    Region region(int tile_x, int tile_y, float w, float h);

    virtual void handle_left_mouse_down(int tile_x, int tile_y);

    virtual void handle_left_mouse_up(int tile_x, int tile_y);

    virtual void handle_right_mouse_down(int tile_x, int tile_y);

private:
    AppState* m_app_state;
    Editor* m_editor;
};

#endif
