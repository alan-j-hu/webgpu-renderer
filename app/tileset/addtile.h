#ifndef ADDTILE_H
#define ADDTILE_H

#include "../modal.h"

/// Modal for adding a new tile.
class AddTile : public Modal
{
public:
    AddTile();

    virtual ModalResponse render() override;
private:
};

#endif
