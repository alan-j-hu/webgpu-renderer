#ifndef MESH_H
#define MESH_H

#include "material.h"
#include "effect.h"
#include <cstddef>
#include <webgpu/webgpu.h>

struct Mesh
{
    WGPUBuffer vertex_buffer;
    WGPUBuffer index_buffer;
    std::size_t vertex_count;
    std::size_t tri_count;

    Mesh(WGPUDevice device,
         Vertex* vertices, std::size_t vertex_count,
         std::uint16_t* indices, std::size_t tri_count);

    ~Mesh();
};

#endif
