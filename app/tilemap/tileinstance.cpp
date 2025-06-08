#include "tileinstance.h"

TileInstance::TileInstance(AppState& state, TileDefinition& definition, int z)
    : m_transform(state.renderer()), m_definition(&definition), m_z(z)
{
}
