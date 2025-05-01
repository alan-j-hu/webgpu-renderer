#include "noworry/meshbuilder.h"

MeshBuilder& MeshBuilder::triangle(XYZ a, XYZ b, XYZ c)
{
    return triangle(
        XYZUV(a.x, a.y, a.z, 0, 0),
        XYZUV(b.x, b.y, b.z, 0, 0),
        XYZUV(c.x, c.y, c.z, 0, 0));
}

MeshBuilder& MeshBuilder::triangle(XYZUV a, XYZUV b, XYZUV c)
{
    int idx = m_vertices.size();
    m_vertices.push_back(a.to_vertex());
    m_vertices.push_back(b.to_vertex());
    m_vertices.push_back(c.to_vertex());

    m_indices.push_back(idx);
    m_indices.push_back(idx + 1);
    m_indices.push_back(idx + 2);

    return *this;
}

MeshBuilder& MeshBuilder::quad(XYZ a, XYZ b, XYZ c, XYZ d)
{
    return quad(
        XYZUV(a.x, a.y, a.z, 0, 0),
        XYZUV(b.x, b.y, b.z, 0, 0),
        XYZUV(c.x, c.y, c.z, 0, 0),
        XYZUV(d.x, d.y, d.z, 0, 0));
}

MeshBuilder& MeshBuilder::quad(XYZUV a, XYZUV b, XYZUV c, XYZUV d)
{
    int idx = m_vertices.size();
    m_vertices.push_back(a.to_vertex());
    m_vertices.push_back(b.to_vertex());
    m_vertices.push_back(c.to_vertex());
    m_vertices.push_back(d.to_vertex());

    m_indices.push_back(idx);
    m_indices.push_back(idx + 1);
    m_indices.push_back(idx + 2);
    m_indices.push_back(idx);
    m_indices.push_back(idx + 2);
    m_indices.push_back(idx + 3);

    return *this;
}

Mesh& MeshBuilder::build(ResourceTable& table)
{
    return table.add_mesh(
        m_vertices.data(), m_vertices.size(),
        m_indices.data(), m_indices.size());
}
