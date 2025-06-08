#ifndef TILEINSTANCE_H
#define TILEINSTANCE_H

#include "../appstate.h"
#include "../tileset/tiledefinition.h"
#include "noworry/transform.h"

class TileInstance
{
public:
    TileInstance(AppState&, TileDefinition&, int z);

    const TileDefinition& definition() const { return *m_definition; }
    TileDefinition& definition() { return *m_definition; }

    Transform& transform() { return m_transform; }

private:
    TileDefinition* m_definition;
    Transform m_transform;
    int m_z;
};

#endif
