#ifndef PLACETILECOMMAND_H
#define PLACETILECOMMAND_H

#include "Command.h"
#include "../Project.h"
#include <optional>
#include <memory>

class PlaceTileCommand : public Command
{
public:
    PlaceTileCommand(
        LayerLocation,
        int x,
        int y,
        int z,
        Rotation rotation,
        std::shared_ptr<TileDef> tiledef);

    const char* name() override;

protected:
    Command::Outcome up(Project&) override;
    void down(Project&) override;

private:
    LayerLocation m_loc;
    int m_x;
    int m_y;
    std::optional<TileInst> m_inst;
};

#endif
