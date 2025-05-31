#ifndef TILESET_TILEROTATIONS_H
#define TILESET_TILEROTATIONS_H

#include "tilemesh.h"

class ModelInspector;

class TileRotations
{
public:
    TileRotations(
        ModelInspector&,
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

    TileMesh rotate(ModelInspector&, int, int, int, int, int, int);

    TileMesh rotate90(ModelInspector& editor, int x, int y);
    TileMesh rotate180(ModelInspector& editor, int x, int y);
    TileMesh rotate270(ModelInspector& editor, int x, int y);
};

#endif
