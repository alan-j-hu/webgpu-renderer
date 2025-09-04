#include "noworry/Gfx3D/ModelData.h"

std::span<const ModelData::Part> ModelData::parts() const
{
    return m_parts;
}

void ModelData::add_part(ModelData::Part part)
{
    m_parts.push_back(std::move(part));
}
