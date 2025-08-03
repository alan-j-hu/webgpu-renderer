#ifndef OBJECT_BIND_GROUP_H
#define OBJECT_BIND_GROUP_H

#include <memory>
#include <vector>
#include <webgpu/webgpu.h>

class Renderer;
class Transform;
class ObjectBindGroupPool;

class ObjectBindGroup
{
public:
    ObjectBindGroup(Renderer&);
    ObjectBindGroup(ObjectBindGroup&&);
    ObjectBindGroup& operator=(ObjectBindGroup&&);
    ~ObjectBindGroup();

    void copy(Renderer& renderer, Transform& transform);
    WGPUBindGroup bind_group() { return m_bind_group; }

private:
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;
};

class ObjectBindGroupPool
{
public:
    ObjectBindGroupPool(Renderer&);

    ObjectBindGroup* alloc();
    void reset();

private:
    Renderer* m_renderer;
    std::vector<std::unique_ptr<ObjectBindGroup>> m_pool;
    int m_index;
};

#endif
