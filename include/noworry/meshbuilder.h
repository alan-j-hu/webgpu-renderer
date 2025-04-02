#ifndef MESHBUILDER_H
#define MESHBUILDER_H

#include "noworry/mesh.h"
#include "noworry/resourcetable.h"
#include <vector>

struct XYZ
{
    explicit XYZ(float x, float y, float z) : x(x), y(y), z(z) {}

    float x;
    float y;
    float z;
};

struct XYZUV
{
    explicit XYZUV(float x, float y, float z, float u, float v)
        : x(x), y(y), z(z), u(u), v(v) {}

    Vertex to_vertex()
    {
        Vertex vertex = {};
        vertex.x = x;
        vertex.y = y;
        vertex.z = z;
        vertex.u = u;
        vertex.v = v;
        return vertex;
    }

    float x;
    float y;
    float z;
    float u;
    float v;
};

class MeshBuilder
{
public:

    MeshBuilder& triangle(XYZ, XYZ, XYZ);
    MeshBuilder& triangle(XYZUV, XYZUV, XYZUV);
    MeshBuilder& quad(XYZ, XYZ, XYZ, XYZ);
    MeshBuilder& quad(XYZUV, XYZUV, XYZUV, XYZUV);
    Mesh& build(ResourceTable& table);

private:

    std::vector<Vertex> m_vertices;
    std::vector<std::uint16_t> m_indices;
};

#endif
