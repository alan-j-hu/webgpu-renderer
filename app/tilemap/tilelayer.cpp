#include "tilelayer.h"

TileLayer::TileLayer()
    : m_tiles(16 * 16, std::nullopt)
{
}

const std::optional<TileInstance>& TileLayer::at(int x, int y) const
{
    return m_tiles.at(y * 16 + x);
}

std::optional<TileInstance>& TileLayer::at(int x, int y)
{
    return m_tiles.at(y * 16 + x);
}

void TileLayer::render(Frame& frame)
{
    int i = 0;
    for (auto& tile : m_tiles) {
        if (tile) {
            int x = i % 16;
            int y = i / 16;
            //tile->transform().set_translation(glm::vec3(x, y, 0));
            frame.add(tile->transform(),
                      tile->definition().tile_mesh().mesh(),
                      *tile->definition().material);
        }
        ++i;
    }
}
