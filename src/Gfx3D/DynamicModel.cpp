#include "noworry/Gfx3D/DynamicModel.h"
#include "noworry/Gfx3D/ModelData.h"
#include <glm/vec4.hpp>

void DynamicModel::add_model(const ModelData& data, const glm::mat4& transform)
{
    std::vector<Vertex> transformed;
    auto parts = data.parts();
    for (auto& part : parts) {
        auto [it, inserted] = m_parts.try_emplace(part.material().get());

        transformed.clear();
        auto vertices = part.vertices();
        for (Vertex v : vertices) {
            glm::vec4 v4 = transform * glm::vec4(v.x, v.y, v.z, 1);
            Vertex new_v;
            new_v.x = v4.x;
            new_v.y = v4.y;
            new_v.z = v4.z;
            new_v.u = v.u;
            new_v.v = v.v;

            transformed.push_back(new_v);
        }

        it->second.append(transformed, part.indices());
    }
}

void DynamicModel::flush(WGPUDevice device)
{
    for (auto it = m_parts.begin(); it != m_parts.end();) {
        if (it->second.vertex_count() == 0 || it->second.index_count() == 0) {
            it = m_parts.erase(it);
        } else {
            it->second.flush(device);
            ++it;
        }
    }
}

void DynamicModel::reset()
{
    for (auto& [material, mesh] : m_parts) {
        mesh.clear();
    }
}

DynamicModel::const_iterator DynamicModel::begin() const
{
    return m_parts.cbegin();
}

DynamicModel::const_iterator DynamicModel::end() const
{
    return m_parts.cend();
}
