#ifndef MODEL_DATA_H
#define MODEL_DATA_H

#include "../Pipeline/MeshVertexShader.h"
#include <cstddef>
#include <memory>
#include <span>
#include <vector>

class Material;

class ModelData
{
public:
    struct Part
    {
    public:
        Part(std::vector<Vertex>,
             std::vector<std::uint16_t>,
             std::shared_ptr<Material>,
             int index_count);

        std::span<const Vertex> vertices() const
        {
            return m_vertices;
        }

        std::span<const std::uint16_t> indices_with_padding() const
        {
            return m_indices;
        }

        std::span<const std::uint16_t> indices() const
        {
            return std::span<const std::uint16_t>(
                m_indices.begin(),
                m_index_count);
        }

        std::shared_ptr<Material> material() const
        {
            return m_material;
        }

    private:
        std::vector<Vertex> m_vertices;
        std::vector<std::uint16_t> m_indices;
        std::shared_ptr<Material> m_material;
        int m_index_count;
    };

    std::span<const Part> parts() const;

    void add_part(Part);

private:
    std::vector<Part> m_parts;
};

#endif
