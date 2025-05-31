#ifndef TILEINSTANCE_H
#define TILEINSTANCE_H

#include "../tileset/tiledefinition.h"

class Position
{
public:
    Position(int x, int y, int z)
    {
        m_x = x;
        m_y = y;
        m_z = z;
    }

    unsigned int x() { return m_x; }
    unsigned int y() { return m_y; }
    unsigned int z() { return m_z; }

private:
    unsigned int m_x : 4;
    unsigned int m_y : 4;
    unsigned char m_z;
};

class TileInstance
{
public:
    TileInstance(Position, TileDefinition&);

    const Position& position() { return m_position; }
    const TileDefinition& definition() const { return *m_definition; }

private:
    Position m_position;
    TileDefinition* m_definition;
};

#endif
