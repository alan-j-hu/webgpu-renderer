#include "tileinstance.h"

TileInstance::TileInstance(
    Position position, TileDefinition& definition)
    : m_position(std::move(position)),
      m_definition(&definition)
{
}
