#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "ModelPart.h"
#include "../Material/Material.h"
#include <memory>
#include <span>
#include <vector>

class Model
{
public:
    std::span<const std::unique_ptr<Mesh>> meshes() const;

    std::span<const std::shared_ptr<Material>> materials() const;

    std::span<const ModelPart> parts() const;

    void add_mesh(std::unique_ptr<Mesh>);

    void add_material(std::shared_ptr<Material>);

    void add_part(ModelPart);

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<std::shared_ptr<Material>> m_materials;
    std::vector<ModelPart> m_parts;
};

#endif
