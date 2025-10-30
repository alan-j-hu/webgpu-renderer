#ifndef VIEW_2D_MODE_H
#define VIEW_2D_MODE_H

#include "Mode.h"

class AppState;
class TilemapEditor;

class View2DMode : public Mode
{
public:
    View2DMode(AppState&, TilemapEditor&);

protected:
    AppState& app_state();

    TilemapEditor& editor();

private:
    AppState* m_app_state;
    TilemapEditor* m_editor;
};

#endif
