#ifndef CREATETILECOMMAND_H
#define CREATETILECOMMAND_H

#include "Command.h"

class CreateTileCommand : public Command
{
public:
    CreateTileCommand(TileDef);

    virtual const char* name() override;

protected:
    virtual Command::Outcome up(Project&) override;
    virtual void down(Project&) override;

private:
    TileDef m_tile_def;
};

#endif
