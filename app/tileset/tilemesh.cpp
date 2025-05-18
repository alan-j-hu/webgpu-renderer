#include "tilemesh.h"
#include "tileseteditor.h"

#include <algorithm>
#include <iterator>
#include <utility>
#include <glm/ext/matrix_transform.hpp>

TileMesh::TileMesh(
    TilesetEditor& editor,
    std::vector<Vertex> vertices,
    std::vector<std::uint16_t> indices)
    : m_vertices(std::move(vertices)),
      m_indices(std::move(indices))
{
    m_mesh = std::make_unique<Mesh>(
        editor.renderer().device(),
        m_vertices.data(), m_vertices.size(),
        m_indices.data(), m_indices.size());

    m_model = std::make_unique<RenderObject>(
        editor.renderer().device(), *m_mesh, editor.default_material());
}

TileMesh TileMesh::rotate90(TilesetEditor& editor, int x, int y)
{
    return rotate(editor, 0, 1, -1, 0, y, 0);
}

TileMesh TileMesh::rotate180(TilesetEditor& editor, int x, int y)
{
    return rotate(editor, -1, 0, 0, -1, x, y);
}

TileMesh TileMesh::rotate270(TilesetEditor& editor, int x, int y)
{
    return rotate(editor, 0, -1, 1, 0, 0, x);
}

TileMesh TileMesh::rotate(
    TilesetEditor& editor,
    int a, int b, int c, int d, int x, int y)
{
    std::vector<Vertex> new_vertices;
    new_vertices.reserve(m_vertices.size());
    std::transform(
        m_vertices.cbegin(),
        m_vertices.cend(),
        std::back_inserter(new_vertices),
        [=](Vertex v)
        {
            glm::mat4 rotation {};
            rotation[0][0] = a;
            rotation[0][2] = b;
            rotation[1][1] = 1;
            rotation[2][0] = c;
            rotation[2][2] = d;
            rotation[3][3] = 1;

            const glm::mat4 translation =
                translate(glm::mat4(1), glm::vec3(x, 0, y));
            const glm::vec4 out =
                translation * rotation * glm::vec4(v.x, v.y, v.z, 1);

            Vertex vout = v;
            vout.x = out[0];
            vout.y = out[1];
            vout.z = out[2];
            return vout;
        });

    return TileMesh(editor, std::move(new_vertices), m_indices);
}
