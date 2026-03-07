#ifndef MOVE_TILEDEF_COMMAND_H
#define MOVE_TILEDEF_COMMAND_H

#include "Command.h"

class MoveTiledefCommand : public Command
{
public:
    MoveTiledefCommand(Tileset&, int, int);

    virtual const char* name() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    Tileset* m_tileset;
    int m_old_idx;
    int m_new_idx;
};

#endif
