#include "tilerotations.h"
#include "../appstate.h"

#include <algorithm>
#include <iterator>
#include <utility>
#include <glm/ext/matrix_transform.hpp>

TileRotations::TileRotations(
    AppState& app_state,
    std::string name,
    std::vector<Vertex> vertices,
    std::vector<std::uint16_t> indices)
    : m_name(std::move(name)),
      m_indices(std::move(indices)),
      m_original(app_state, std::move(vertices), m_indices),
      m_rotated90(rotate90(app_state, 1, 1)),
      m_rotated180(rotate180(app_state, 1, 1)),
      m_rotated270(rotate270(app_state, 1, 1))
{
}

TileMesh TileRotations::rotate(
    AppState& app_state,
    int a, int b, int c, int d, int x, int y)
{
    std::vector<Vertex> new_vertices;
    new_vertices.reserve(m_original.vertices().size());
    std::transform(
        m_original.vertices().cbegin(),
        m_original.vertices().cend(),
        std::back_inserter(new_vertices),
        [=](Vertex v)
        {
            glm::mat4 rotation {};
            // Column index goes first
            rotation[0][0] = a;
            rotation[1][0] = b;
            rotation[0][1] = c;
            rotation[1][1] = d;
            rotation[2][2] = 1;
            rotation[3][3] = 1;

            const glm::mat4 translation =
                translate(glm::mat4(1), glm::vec3(x, y, 0));
            const glm::vec4 out =
                translation * rotation * glm::vec4(v.x, v.y, v.z, 1);

            Vertex vout = v;
            vout.x = out[0];
            vout.y = out[1];
            vout.z = out[2];
            return vout;
        });

    return TileMesh(app_state, std::move(new_vertices), m_indices);
}

TileMesh TileRotations::rotate90(AppState& state, int x, int y)
{
    return rotate(state, 0, -1, 1, 0, y, 0);
}

TileMesh TileRotations::rotate180(AppState& state, int x, int y)
{
    return rotate(state, -1, 0, 0, -1, x, y);
}

TileMesh TileRotations::rotate270(AppState& state, int x, int y)
{
    return rotate(state, 0, 1, -1, 0, 0, x);
}
