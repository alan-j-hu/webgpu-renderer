#ifndef MESH_H
#define MESH_H

#include <webgpu/webgpu.h>

class Mesh
{
public:
    virtual WGPUBuffer vertex_buffer() const = 0;
    virtual WGPUBuffer index_buffer() const = 0;
    virtual std::size_t vertex_count() const = 0;
    virtual std::size_t index_count() const = 0;
    virtual WGPUPrimitiveTopology topology() const = 0;
};

#endif
