#ifndef TILEINSTANCE_H
#define TILEINSTANCE_H

#include "../appstate.h"
#include "../tileset/tiledefinition.h"

class TileInstance
{
public:
    TileInstance(AppState&, TileDefinition&, int z);

    const TileDefinition& definition() const { return *m_definition; }
    TileDefinition& definition() { return *m_definition; }

private:
    TileDefinition* m_definition;
    int m_z;
};

#endif
