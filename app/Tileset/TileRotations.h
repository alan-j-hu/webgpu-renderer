#ifndef TILESET_TILEROTATIONS_H
#define TILESET_TILEROTATIONS_H

#include "RotationDropdown.h"
#include "TileMesh.h"

class AppState;

/// Contains the four rotated tile meshes for a given tine shape.
class TileRotations
{
public:
    TileRotations(
        AppState&,
        std::string,
        std::vector<Vertex>,
        std::vector<std::uint16_t>);

    const std::string& name() { return m_name; }

    TileMesh& rotated0() { return m_original; }
    TileMesh& rotated90() { return m_rotated90; }
    TileMesh& rotated180() { return m_rotated180; }
    TileMesh& rotated270() { return m_rotated270; }

    TileMesh& rotated(RotationTag tag);

private:
    std::string m_name;
    std::vector<std::uint16_t> m_indices;

    TileMesh m_original;
    TileMesh m_rotated90;
    TileMesh m_rotated180;
    TileMesh m_rotated270;

    TileMesh rotate(AppState&, int, int, int, int, int, int);

    TileMesh rotate90(AppState& editor, int x, int y);
    TileMesh rotate180(AppState& editor, int x, int y);
    TileMesh rotate270(AppState& editor, int x, int y);
};

#endif
