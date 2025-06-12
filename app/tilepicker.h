#ifndef TILEPICKER_H
#define TILEPICKER_H

#include "appstate.h"

class TilePicker
{
public:
    TilePicker(AppState&);

    void render(int& selected_tile);

private:
    AppState* m_app_state;
};

#endif
