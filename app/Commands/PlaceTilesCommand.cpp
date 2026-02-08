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

auto PlaceTilesCommand::up(
    Project& project
) -> std::expected<Command::Outcome, std::string>
{
    bool changed = false;
    for (auto& pair : m_old_tiles) {
        if (m_inst != pair.second) {
            m_layer.set(pair.first.x, pair.first.y, m_inst);
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
    for (auto& pair : m_old_tiles) {
        m_layer.set(pair.first.x, pair.first.y, pair.second);
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
}
