#ifndef CREATE_TILEDEF_COMMAND_H
#define CREATE_TILEDEF_COMMAND_H

#include "Command.h"

class CreateTiledefCommand : public Command
{
public:
    CreateTiledefCommand(TileDef, int index);

    const char* undo_string() override;
    const char* redo_string() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    TileDef m_tiledef;
    int m_index;
};

#endif
