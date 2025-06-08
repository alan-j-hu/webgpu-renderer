#include "tilelayer.h"

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
    for (auto& tile : m_tiles) {
        if (tile) {
            frame.add(tile->transform(),
                      tile->definition().tile_mesh().mesh(),
                      *tile->definition().material);
        }
    }
}
