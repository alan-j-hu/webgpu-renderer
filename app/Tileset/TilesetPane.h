#ifndef TILESET_PANE_H
#define TILESET_PANE_H

#include "../AppState.h"
#include "../Pane.h"
#include "TileList.h"

/// The sub-editor for editing the tileset.
class TilesetPane : public HorizontalLayout
{
public:
    TilesetPane(int flex, AppState&);

private:
    TileList m_tile_list;
};

#endif
