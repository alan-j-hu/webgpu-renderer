#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "ModelData.h"
#include "../Material/Material.h"
#include <memory>
#include <span>
#include <vector>

class Model
{
public:
    struct Part
    {
        Part(std::unique_ptr<Mesh>, std::shared_ptr<Material>);

        const Mesh& mesh() const;

        const Material& material() const;

    private:
        std::unique_ptr<Mesh> m_mesh;
        std::shared_ptr<Material> m_material;
    };

    Model() = default;

    Model(WGPUDevice device, const ModelData&);

    std::span<const Part> parts() const;

    void add_part(Part);

private:
    std::vector<Part> m_parts;
};

#endif
