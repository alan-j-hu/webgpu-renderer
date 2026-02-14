#ifndef UPDATE_TILE_COMMAND_H
#define UPDATE_TILE_COMMAND_H

#include "Command.h"

class UpdateTileCommand : public Command
{
public:
    UpdateTileCommand(int, TileDef);

    const char* name() override;

protected:
    std::expected<Outcome, std::string> up(Project&) override;
    void down(Project&) override;

private:
    TileDef m_tiledef;
    int m_idx;
};

#endif
