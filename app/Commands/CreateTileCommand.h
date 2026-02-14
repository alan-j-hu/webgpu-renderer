#ifndef CREATE_TILE_COMMAND_H
#define CREATE_TILE_COMMAND_H

#include "Command.h"

class CreateTileCommand : public Command
{
public:
    CreateTileCommand(TileDef, int index);

    virtual const char* name() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    TileDef m_tiledef;
    int m_index;
};

#endif
