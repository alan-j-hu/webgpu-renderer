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

    TileMesh& rotate0() { return m_original; }
    TileMesh& rotate90() { return m_rotate90; }
    TileMesh& rotate180() { return m_rotate180; }
    TileMesh& rotate270() { return m_rotate270; }

private:
    std::string m_name;

    TileMesh m_original;
    TileMesh m_rotate90;
    TileMesh m_rotate180;
    TileMesh m_rotate270;
};

#endif
