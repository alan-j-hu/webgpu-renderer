#ifndef MESH_H
#define MESH_H

#include "material.h"
#include "pipeline.h"
#include <cstddef>
#include <webgpu/webgpu.h>

struct Mesh
{
    WGPUBuffer vertex_buffer;
    std::size_t tri_count;
    Material* material;

    Mesh(WGPUDevice device,
         Vertex* vertices, std::size_t count, Material& material);

    ~Mesh();
};

#endif
