#include "PlaceTilesCommand.h"

PlaceTilesCommand::PlaceTilesCommand(
    Layer& layer,
    short z,
    Rotation rotation,
    std::shared_ptr<const TileDef> tiledef)
    : m_layer(layer),
      m_inst(std::make_optional<TileInst>(tiledef, z, rotation))
{
}

PlaceTilesCommand::PlaceTilesCommand(Layer& layer)
    : m_layer(layer)
{
}

Command::Outcome PlaceTilesCommand::up(Project& project)
{
    bool changed = false;
    for (auto& placement : m_placements) {
        auto old = m_old_tiles[placement];
        if (m_inst != old) {
            m_layer.set(placement.x, placement.y, m_inst);
            changed = true;
        }
    }

    if (!changed) {
        return Outcome::UNCHANGED;
    }
    return Outcome::IN_PROGRESS;
}

void PlaceTilesCommand::down(Project& project)
{
    for (auto& placement : m_placements) {
        auto old = m_old_tiles[placement];
        m_layer.set(placement.x, placement.y, old);
    }
}

const char* PlaceTilesCommand::name()
{
    return "Place Tile";
}

void PlaceTilesCommand::add_placement(int x, int y)
{
    auto ivec2 = glm::ivec2(x, y);
    auto it = m_old_tiles.find(ivec2);
    if (it == m_old_tiles.end()) {
        std::optional<TileInst> old = m_layer.at(x, y);
        m_old_tiles.insert(std::pair(ivec2, old));
    }

    m_placements.emplace_back(x, y);
}
