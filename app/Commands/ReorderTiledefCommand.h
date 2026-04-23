#ifndef REORDER_TILEDEF_COMMAND_H
#define REORDER_TILEDEF_COMMAND_H

#include "Command.h"

class ReorderTiledefCommand : public Command
{
public:
    ReorderTiledefCommand(Tileset&, int, int);

    const char* undo_string() override;
    const char* redo_string() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    Tileset* m_tileset;
    int m_idx;
    int m_delta;
};

#endif
