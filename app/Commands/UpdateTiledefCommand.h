#ifndef UPDATE_TILEDEF_COMMAND_H
#define UPDATE_TILEDEF_COMMAND_H

#include "Command.h"

class UpdateTiledefCommand : public Command
{
public:
    UpdateTiledefCommand(int, TileDef);

    const char* name() override;

protected:
    std::expected<Outcome, std::string> up(Project&) override;
    void down(Project&) override;

private:
    TileDef m_tiledef;
    int m_idx;
};

#endif
