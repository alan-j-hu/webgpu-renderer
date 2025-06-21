#ifndef PIPELINEFACTORY_H
#define PIPELINEFACTORY_H

#include "MeshVertexShader.h"
#include "../Material/Effect.h"

#include "../scene/renderobject.h"
#include <unordered_map>
#include <vector>

struct PipelineKey
{
    MeshVertexShader* vertex_shader;
    Effect* effect;
    WGPUPrimitiveTopology topology;
    bool operator==(const PipelineKey&) const = default;
};

template<>
struct std::hash<PipelineKey>
{
    std::size_t operator()(const PipelineKey& k) const noexcept
    {
        auto h1 = std::hash<MeshVertexShader*>{}(k.vertex_shader);
        auto h2 = std::hash<Effect*>{}(k.effect);
        auto h3 = std::hash<WGPUPrimitiveTopology>{}(k.topology);
        return h1 ^ (h2 << 1) ^ (h3 << 3);
    }
};

/// A pipeline contains a WGPURenderPipeline, its layout, and a render queue.
class Pipeline
{
public:
    Pipeline(WGPUDevice device, const PipelineKey&);
    Pipeline(const Pipeline& other) = delete;
    Pipeline(Pipeline&&);

    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&);

    ~Pipeline();

    bool queued() const { return m_queued; }
    void set_queued(bool b) { m_queued = b; }

    void enqueue(RenderObject);

    void draw(WGPURenderPassEncoder encoder);

private:
    WGPUPipelineLayout m_pipeline_layout;
    WGPURenderPipeline m_pipeline;
    std::vector<RenderObject> m_render_queue;
    bool m_queued;
};

class Renderer;

/// A pipeline factory stores a pipeline cache.
class PipelineFactory
{
public:
    Pipeline& get_pipeline(WGPUDevice device, const PipelineKey&);

    void enqueue(Pipeline&);
    void enqueue(Renderer& renderer, RenderObject ro);

    void draw(WGPURenderPassEncoder encoder);

private:
    std::unordered_map<PipelineKey, Pipeline> m_pipelines;
    std::vector<Pipeline*> m_pipeline_queue;
};

#endif
