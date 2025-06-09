#ifndef TILEMAP_TILELAYER_H
#define TILEMAP_TILELAYER_H

#include "tileinstance.h"
#include "noworry/renderer.h"
#include <optional>
#include <vector>

class TileLayer
{
public:
    TileLayer();

    const std::optional<TileInstance>& at(int x, int y) const;
    std::optional<TileInstance>& at(int x, int y);

    void render(Frame&);

private:
    std::vector<std::optional<TileInstance>> m_tiles;
};

#endif
