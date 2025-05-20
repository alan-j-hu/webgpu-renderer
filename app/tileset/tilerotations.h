#ifndef TILESET_TILEROTATIONS_H
#define TILESET_TILEROTATIONS_H

#include "tilemesh.h"

class TileRotations
{
public:
    TileRotations(
        TilesetEditor&,
        std::string,
        std::vector<Vertex>,
        std::vector<std::uint16_t>);

    const std::string& name() { return m_name; }

    TileMesh& rotated0() { return m_original; }
    TileMesh& rotated90() { return m_rotated90; }
    TileMesh& rotated180() { return m_rotated180; }
    TileMesh& rotated270() { return m_rotated270; }

private:
    std::string m_name;
    std::vector<std::uint16_t> m_indices;

    TileMesh m_original;
    TileMesh m_rotated90;
    TileMesh m_rotated180;
    TileMesh m_rotated270;

    TileMesh rotate(TilesetEditor&, int, int, int, int, int, int);

    TileMesh rotate90(TilesetEditor& editor, int x, int y);
    TileMesh rotate180(TilesetEditor& editor, int x, int y);
    TileMesh rotate270(TilesetEditor& editor, int x, int y);
};

#endif
