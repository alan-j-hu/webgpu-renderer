#ifndef TILEPICKER_H
#define TILEPICKER_H

#include "AppState.h"

class Editor;

class TilePicker
{
public:
    TilePicker(AppState&, Editor&);

    void render(std::optional<int>& selected_tile);

private:
    AppState* m_app_state;
    Editor* m_editor;
};

#endif
