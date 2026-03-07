#ifndef DELETE_TILEDEF_COMMAND_H
#define DELETE_TILEDEF_COMMAND_H

#include "Command.h"
#include <memory>

class DeleteTiledefCommand : public Command
{
public:
    DeleteTiledefCommand(Tileset&, int index);

    virtual const char* name() override;

protected:
    virtual std::expected<Outcome, std::string> up(Project&) override;
    virtual void down(Project&) override;

private:
    std::shared_ptr<TileDef> m_tiledef;
    Tileset* m_tileset;
    int m_index;
};

#endif
