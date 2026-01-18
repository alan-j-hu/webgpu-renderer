#ifndef PLACETILECOMMAND_H
#define PLACETILECOMMAND_H

#include "Command.h"
#include "../Project.h"
#include <optional>
#include <memory>
#include <unordered_map>
#include <vector>

class PlaceTileCommand : public Command
{
public:
    PlaceTileCommand(
        Layer&,
        short z,
        Rotation rotation,
        std::shared_ptr<const TileDef> tiledef);

    const char* name() override;

    void add_placement(int x, int y);

protected:
    Command::Outcome up(Project&) override;
    void down(Project&) override;

private:
    // SAFETY: m_layer is a live reference because the underlying object
    // will either be owned by the project state or another command in the
    // undo/redo stack.
    Layer& m_layer;
    std::optional<TileInst> m_inst;

    std::unordered_map<glm::ivec2, std::optional<TileInst>> m_old_tiles;
    std::vector<glm::ivec2> m_placements;
};

#endif
