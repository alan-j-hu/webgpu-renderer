#ifndef MODEL_NODE_H
#define MODEL_NODE_H

#include <memory>
#include <vector>
#include <glm/mat4x4.hpp>

struct ModelPart
{
    ModelPart(int mesh, int material, const glm::mat4& transform);

    int mesh() const;

    int material() const;

    const glm::mat4& transform() const;
    glm::mat4& transform();

private:
    glm::mat4 m_transform;
    int m_mesh;
    int m_material;
};

#endif
