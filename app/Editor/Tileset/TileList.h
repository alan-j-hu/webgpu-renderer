#ifndef TILE_LIST_H
#define TILE_LIST_H

#include "NewTile.h"
#include "TileDefinitionEditor.h"
#include "../ListBox.h"
#include "../../AppState.h"

class Editor;

/// Edits the list of tiles.
class TileList : public ListBox
{
public:
    TileList(AppState&, Editor&);

    virtual int count() override;

    virtual ItemState item(int idx) override;

    virtual void select(int idx) override;

    virtual void request_reorder(int old_idx, int new_idx) override;

    void draw();

private:
    AppState* m_app_state;
    Editor* m_editor;
    NewTile m_new_tile;
    TileDefinitionEditor m_tile_editor;

    void context_menu(int idx);
};

#endif
