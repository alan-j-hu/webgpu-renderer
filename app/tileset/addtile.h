#ifndef ADDTILE_H
#define ADDTILE_H

#include "tiledefinition.h"
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
    TileDefinition m_definition;
    TileDefinitionEditor m_edit;
};

#endif
