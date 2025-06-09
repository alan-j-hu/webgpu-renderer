#ifndef RENDERER_H
#define RENDERER_H

#include "layout.h"
#include "mesh.h"
#include "texture.h"
#include "material/flatmesheffect.h"
#include "material/mesheffect.h"
#include "material/material.h"

#include <memory>
#include <vector>

class Camera;
class RenderObject;
class RenderTarget;
class Renderer;
class Scene;
class Transform;

/// Builds a single frame. Create a new Frame object for each render frame.
class Frame
{
public:
    Frame(Renderer& renderer, RenderTarget& target, Scene& scene);

    /// The destructor submits frame data to the renderer.
    ~Frame();

    Frame& add(Transform&, const Mesh&, Material&);

private:
    Renderer* m_renderer;
    RenderTarget* m_target;
    Scene* m_scene;
};

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

/// Renderer. The same renderer can be used for multiple scenes and
/// render targets, so one Renderer object should generally exist in the
/// entire program.
class Renderer
{
public:
    Renderer(WGPUDevice device);
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    virtual ~Renderer();

    WGPUDevice& device() { return m_device; }

    template<class T>
    T* mesh_effect()
    {
        for (auto& effect : m_mesh_effects) {
            if (auto casted = dynamic_cast<T*>(effect.get())) {
                if (typeid(*casted) == typeid(T)) {
                    return casted;
                }
            }
        }
        return nullptr;
    }

    WGPUSampler default_sampler() { return m_sampler; }

    UniformLayout& uniform_layout() { return m_uniform_layout; }

    ModelGroup& alloc_group();

    void render(RenderTarget&, Scene& scene);

private:
    WGPUDevice m_device;
    UniformLayout m_uniform_layout;
    WGPUSampler m_sampler;
    std::vector<std::unique_ptr<MeshEffect>> m_mesh_effects;
    std::vector<std::unique_ptr<ModelGroup>> m_model_groups;
    int m_next_group;

    void do_render(WGPURenderPassEncoder encoder);
};

#endif
