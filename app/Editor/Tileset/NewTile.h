#ifndef NEW_TILE_H
#define NEW_TILE_H

#include "TileDefinitionEditor.h"
#include "../FileDialog.h"
#include "../../AppState.h"

/// Modal for adding a new tile.
class NewTile
{
public:
    NewTile(AppState&);

    void open(int index);

    void update();

private:
    AppState* m_app_state;
    std::unique_ptr<TileDef> m_definition;
    TileDefinitionEditor m_edit;
    int m_index;
};

#endif
