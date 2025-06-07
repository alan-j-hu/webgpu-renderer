#include "noworry/grid.h"
#include <vector>

void create_parallel_lines(
    std::vector<Vertex>& vertices,
    const glm::vec3& pos,
    int w,
    int h,
    const glm::vec3& axis1,
    const glm::vec3& axis2)
{
    float wf = w;
    for (int i = 0; i <= w; ++i) {
        glm::vec3 p1 = pos + (i / wf) * axis1;
        glm::vec3 p2 = p1 + axis2;
        Vertex v1;
        v1.x = p1.x;
        v1.y = p1.y;
        v1.z = p1.z;
        v1.u = 0;
        v1.v = 0;

        Vertex v2;
        v2.x = p2.x;
        v2.y = p2.y;
        v2.z = p2.z;
        v2.u = 0;
        v2.v = 0;
        vertices.push_back(v1);
        vertices.push_back(v2);
    }
}

Mesh create_grid(
    WGPUDevice device,
    const glm::vec3& pos,
    int w,
    int h,
    const glm::vec3& axis1,
    const glm::vec3& axis2)
{
    std::vector<Vertex> vertices;
    create_parallel_lines(vertices, pos, w, h, axis1, axis2);
    create_parallel_lines(vertices, pos, h, w, axis2, axis1);

    std::vector<std::uint16_t> indices;
    for (int i = 0; i < vertices.size(); ++i) {
        indices.push_back(i);
    }

    return
        Mesh(device,
             vertices.data(), vertices.size(),
             indices.data(), indices.size(),
             indices.size());
}
