#ifndef MODEL_GROUP_H
#define MODEL_GROUP_H

#include <webgpu/webgpu.h>

class Transform;
class Renderer;

/// Holds the buffer and bind group for the model transform. The renderer
/// maintains these objects in a pool.
class ModelGroup
{
public:
    ModelGroup(Renderer&);
    ModelGroup(const ModelGroup&) = delete;
    ModelGroup& operator=(const ModelGroup&) = delete;
    ModelGroup(ModelGroup&&);
    ModelGroup& operator=(ModelGroup&&);
    ~ModelGroup();

    void copy(Renderer& renderer, Transform& transform);
    WGPUBindGroup bind_group() { return m_bind_group; }

private:
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;
};

#endif
