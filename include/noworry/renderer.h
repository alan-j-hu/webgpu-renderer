#ifndef RENDERER_H
#define RENDERER_H

#include "texture.h"
#include "Gfx3D/BasicMesh.h"
#include "Gfx3D/ObjectBindGroup.h"
#include "Gfx3D/RenderBatch.h"
#include "Material/Effect.h"
#include "Material/FlatEffect.h"
#include "Material/Material.h"
#include "Pipeline/MeshVertexShader.h"
#include "Pipeline/PipelineFactory.h"

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

    Frame& add(Transform&, const BasicMesh&, Material&);

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
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;
    virtual ~Renderer();

    WGPUDevice& device() { return m_device; }

    template<class T>
    T* effect()
    {
        for (auto& effect : m_effects) {
            if (auto casted = dynamic_cast<T*>(effect.get())) {
                if (typeid(*casted) == typeid(T)) {
                    return casted;
                }
            }
        }
        return nullptr;
    }

    WGPUSampler default_sampler() { return m_sampler; }

    const MeshVertexShader& mesh_vertex_shader() const
    {
        return m_mesh_vertex_shader;
    }

    MeshVertexShader& mesh_vertex_shader()
    {
        return m_mesh_vertex_shader;
    }

    PipelineFactory& pipeline_factory() { return m_pipeline_factory; }

    ObjectBindGroupPool& bind_group_pool() { return m_object_group_pool; }

    void render(RenderTarget&, Scene& scene);

private:
    WGPUDevice m_device;
    WGPUSampler m_sampler;
    MeshVertexShader m_mesh_vertex_shader;
    PipelineFactory m_pipeline_factory;
    std::vector<std::unique_ptr<Effect>> m_effects;

    ObjectBindGroupPool m_object_group_pool;
    RenderBatcher m_batcher;

    void do_render(Scene& scene, WGPURenderPassEncoder encoder);
};

#endif
