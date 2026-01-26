#include "noworry/Gfx3D/ModelData.h"
#include <utility>

ModelData::Part::Part(std::shared_ptr<Material> material)
    : m_material(std::move(material))
{
}

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

ModelData::Part::Part(ModelData::Part&& other)
    : m_vertices(std::move(other.m_vertices)),
      m_indices(std::move(other.m_indices)),
      m_material(std::move(other.m_material)),
      m_index_count {other.m_index_count}
{
    other.m_index_count = 0;
}

ModelData::Part& ModelData::Part::operator=(ModelData::Part&& other)
{
    m_vertices = std::move(other.m_vertices);
    m_indices = std::move(other.m_indices);
    m_material = std::move(other.m_material);
    m_index_count = std::exchange(other.m_index_count, 0);

    return *this;
}

std::span<const Vertex> ModelData::Part::vertices() const
{
    return m_vertices;
}

std::span<const std::uint16_t> ModelData::Part::indices_with_padding() const
{
    return m_indices;
}

std::span<const std::uint16_t> ModelData::Part::indices() const
{
    return std::span<const std::uint16_t>(
        m_indices.begin(),
        m_index_count);
}

std::shared_ptr<Material> ModelData::Part::material() const
{
    return m_material;
}

void ModelData::Part::clear()
{
    m_vertices.clear();
    m_indices.clear();
    m_index_count = 0;
}

void ModelData::Part::append(
    std::span<const Vertex> vertices,
    std::span<const std::uint16_t> indices)
{
    std::size_t vertex_offset = m_vertices.size();
    m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());

    m_indices.resize(m_index_count);
    m_indices.insert(m_indices.end(), indices.begin(), indices.end());
    for (int i = m_index_count; i < m_indices.size(); ++i) {
        m_indices.at(i) += vertex_offset;
    }

    m_index_count = m_indices.size();
    if (m_indices.size() & 1 != 0) {
        m_indices.push_back(0);
    }
}

void ModelData::Part::append(const Part& other)
{
    append(other.vertices(), other.indices());
}

std::span<const ModelData::Part> ModelData::parts() const
{
    return m_parts;
}

void ModelData::add_part(ModelData::Part part)
{
    m_parts.push_back(std::move(part));
}
