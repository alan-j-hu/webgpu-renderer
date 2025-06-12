#ifndef UPDATETILECOMMAND_H
#define UPDATETILECOMMAND_H

#include "Command.h"

class UpdateTileCommand : public Command
{
public:
    UpdateTileCommand(int, TileDef);

    const char* name() override;

protected:
    void up(Project&) override;
    void down(Project&) override;

private:
    TileDef m_tiledef;
    int m_idx;
};

#endif
