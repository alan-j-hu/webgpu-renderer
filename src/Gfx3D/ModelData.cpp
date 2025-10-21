#include "noworry/Gfx3D/ModelData.h"

ModelData::Part::Part(std::vector<Vertex> vertices,
                      std::vector<std::uint16_t> indices,
                      std::shared_ptr<Material> material,
                      int index_count)
    : m_vertices(std::move(vertices)),
      m_indices(std::move(indices)),
      m_material(std::move(material)),
      m_index_count(index_count)
{

}

std::span<const ModelData::Part> ModelData::parts() const
{
    return m_parts;
}

void ModelData::add_part(ModelData::Part part)
{
    m_parts.push_back(std::move(part));
}
