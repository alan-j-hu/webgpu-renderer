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
        Layer&,
        int x,
        int y,
        int z,
        Rotation rotation,
        std::shared_ptr<const TileDef> tiledef);

    const char* name() override;

protected:
    Command::Outcome up(Project&) override;
    void down(Project&) override;

private:
    // SAFETY: m_layer is a live reference because the underlying object
    // will either be owned by the project state or another command in the
    // undo/redo stack.
    Layer& m_layer;
    int m_x;
    int m_y;
    std::optional<TileInst> m_inst;
};

#endif
