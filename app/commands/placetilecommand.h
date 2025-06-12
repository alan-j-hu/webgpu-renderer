#ifndef PLACETILECOMMAND_H
#define PLACETILECOMMAND_H

#include "command.h"
#include <optional>
#include <memory>

class PlaceTileCommand : public Command
{
public:
    PlaceTileCommand(int idx, int x, int y, int z, std::shared_ptr<TileDef>);

    const char* name() override;

protected:
    void up(Project&) override;
    void down(Project&) override;

private:
    int m_layer_idx;
    int m_x;
    int m_y;
    std::optional<TileInst> m_inst;
};

#endif
