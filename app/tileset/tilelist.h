#ifndef TILELIST_H
#define TILELIST_H

#include "tiledefinitioneditor.h"
#include "../appstate.h"
#include "../tilepicker.h"
#include "../pane.h"

/// Edits the list of tiles.
class TileList : public Pane
{
public:
    TileList(std::string name, int flex, AppState&);

protected:
    virtual void content() override;

private:
    AppState* m_app_state;
    TilePicker m_tile_picker;
    TileDefinitionEditor m_editor;
    int m_selected;
};

#endif
