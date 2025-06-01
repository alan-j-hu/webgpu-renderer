#ifndef TILELIST_H
#define TILELIST_H

#include "../appstate.h"
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
};

#endif
