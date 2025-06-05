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

/// Renderer. The same renderer can be used for multiple scenes and
/// render targets, so one Renderer object should generally exist in the
/// entire program.
class Renderer
{
public:
    Renderer(WGPUDevice device);

    virtual ~Renderer();

    WGPUDevice device() { return m_device; }

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

    void render(RenderTarget&, Scene& scene);

private:
    WGPUDevice m_device;
    UniformLayout m_uniform_layout;
    WGPUSampler m_sampler;
    std::vector<std::unique_ptr<MeshEffect>> m_mesh_effects;

    void do_render(WGPURenderPassEncoder encoder);
};

#endif
