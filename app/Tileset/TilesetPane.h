#ifndef TILESET_PANE_H
#define TILESET_PANE_H

#include "../AppState.h"
#include "../Pane.h"
#include "ModelInspector.h"
#include "TileList.h"

/// The sub-editor for editing the tileset.
class TilesetPane : public HorizontalLayout
{
public:
    TilesetPane(int flex, AppState&);

    ModelInspector& editor() { return m_editor; }

private:
    ModelInspector m_editor;
    TileList m_tile_list;
};

#endif
