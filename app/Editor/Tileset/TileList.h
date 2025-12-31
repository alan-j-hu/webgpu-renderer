#ifndef TILELIST_H
#define TILELIST_H

#include "TileDefinitionEditor.h"
#include "../../AppState.h"
#include "../../TilePicker.h"

/// Edits the list of tiles.
class TileList
{
public:
    TileList(AppState&);

    void draw();

private:
    AppState* m_app_state;
    TilePicker m_tile_picker;
    TileDefinitionEditor m_editor;
    int m_selected;
};

#endif
