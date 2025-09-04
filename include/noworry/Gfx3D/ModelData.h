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
        std::vector<Vertex> vertices;
        std::vector<std::uint16_t> indices;
        std::shared_ptr<Material> material;
        int index_count;
    };

    std::span<const Part> parts() const;

    void add_part(Part);

private:
    std::vector<Part> m_parts;
};

#endif
