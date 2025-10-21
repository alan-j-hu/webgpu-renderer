#include "noworry/Gfx3D/Model.h"
#include "noworry/Gfx3D/BasicMesh.h"
#include <utility>

Model::Part::Part(std::unique_ptr<Mesh> mesh,
                  std::shared_ptr<Material> material)
    : m_mesh(std::move(mesh)), m_material(std::move(material))
{
}

const Mesh& Model::Part::mesh() const
{
    return *m_mesh;
}

const Material& Model::Part::material() const
{
    return *m_material;
}

Model::Model(WGPUDevice device, const ModelData& data)
{
    auto data_parts = data.parts();
    for (auto& data_part : data_parts) {
        std::unique_ptr<Mesh> mesh = std::make_unique<BasicMesh>(
              device,
              WGPUPrimitiveTopology_TriangleList,
              data_part.vertices(),
              data_part.indices_with_padding(),
              data_part.indices_with_padding().size());

        Part part(std::move(mesh), data_part.material());
        add_part(std::move(part));
    }
}

std::span<const Model::Part> Model::parts() const
{
    return m_parts;
}

void Model::add_part(Model::Part part)
{
    m_parts.push_back(std::move(part));
}
