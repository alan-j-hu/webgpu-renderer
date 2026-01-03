#ifndef TILE_LIST_H
#define TILE_LIST_H

#include "NewTile.h"
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
    NewTile m_new_tile;
    TileDefinitionEditor m_tile_editor;
    int m_selected;
};

#endif
