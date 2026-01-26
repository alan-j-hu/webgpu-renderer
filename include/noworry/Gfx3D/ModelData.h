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
    /// Model indices are 2 bytes, but WebGPU requires the length of buffer
    /// writes to be a multiple of 4 bytes, so the index vector has padding
    /// when its length is odd.
    struct Part
    {
    public:
        Part() = default;
        Part(std::shared_ptr<Material>);
        Part(std::vector<Vertex>,
             std::vector<std::uint16_t>,
             std::shared_ptr<Material>,
             int index_count);
        Part(Part&&);

        Part& operator=(Part&&);

        std::span<const Vertex> vertices() const;

        std::span<const std::uint16_t> indices_with_padding() const;

        std::span<const std::uint16_t> indices() const;

        std::shared_ptr<Material> material() const;

        void clear();

        void append(std::span<const Vertex>, std::span<const std::uint16_t>);

        void append(const Part& other);

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
