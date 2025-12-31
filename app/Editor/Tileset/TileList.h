#ifndef TILELIST_H
#define TILELIST_H

#include "TileDefinitionEditor.h"
#include "../../AppState.h"
#include "../../TilePicker.h"

class Editor;

/// Edits the list of tiles.
class TileList
{
public:
    TileList(AppState&, Editor&);

    void draw();

private:
    AppState* m_app_state;
    Editor* m_editor;
    TilePicker m_tile_picker;
    TileDefinitionEditor m_tile_editor;
    int m_selected;
};

#endif
