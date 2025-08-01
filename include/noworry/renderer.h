#ifndef RENDERER_H
#define RENDERER_H

#include "mesh.h"
#include "texture.h"
#include "Draw3D/Frame3D.h"
#include "Draw3D/ModelGroup.h"
#include "Material/Effect.h"
#include "Material/FlatEffect.h"
#include "Material/Material.h"
#include "Pipeline/MeshVertexShader.h"
#include "Pipeline/PipelineFactory.h"

#include <memory>
#include <optional>
#include <vector>

class Camera;
class RenderObject;
class RenderTarget;
class Renderer;
class Scene;

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

    ModelGroup& alloc_group();

    void render(RenderTarget&, Scene& scene);

private:
    WGPUDevice m_device;
    WGPUSampler m_sampler;
    MeshVertexShader m_mesh_vertex_shader;
    PipelineFactory m_pipeline_factory;
    std::vector<std::unique_ptr<Effect>> m_effects;
    std::vector<std::unique_ptr<ModelGroup>> m_model_groups;
    int m_next_group;

    std::optional<Frame3D> m_frame;

    void do_render(WGPURenderPassEncoder encoder);
};

#endif
