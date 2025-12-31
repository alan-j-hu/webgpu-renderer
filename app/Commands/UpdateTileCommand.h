#ifndef UPDATETILECOMMAND_H
#define UPDATETILECOMMAND_H

#include "Command.h"
#include <memory>

class UpdateTileCommand : public Command
{
public:
    UpdateTileCommand(int, std::shared_ptr<TileDef>);

    const char* name() override;

protected:
    Command::Outcome up(Project&) override;
    void down(Project&) override;

private:
    std::shared_ptr<TileDef> m_tiledef;
    int m_idx;
};

#endif
