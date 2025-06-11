#ifndef ADDTILE_H
#define ADDTILE_H

#include "tiledefinitioneditor.h"
#include "../appstate.h"
#include "../modal.h"

/// Modal for adding a new tile.
class AddTile : public Modal
{
public:
    AddTile(AppState&);

    virtual ModalResponse render() override;

private:
    AppState* m_app_state;
    TileDef m_definition;
    TileDefinitionEditor m_edit;
};

#endif
