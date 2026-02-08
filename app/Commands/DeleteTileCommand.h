#ifndef DELETE_TILE_COMMAND_H
#define DELETE_TILE_COMMAND_H

#include "Command.h"
#include <memory>

class DeleteTileCommand : public Command
{
public:
    DeleteTileCommand(int index);

    virtual const char* name() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    std::shared_ptr<TileDef> m_tiledef;
    int m_index;
};

#endif
