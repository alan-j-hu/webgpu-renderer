#ifndef PIPELINEFACTORY_H
#define PIPELINEFACTORY_H

#include "MeshVertexShader.h"
#include "../Material/Effect.h"

#include "../scene/renderobject.h"
#include <unordered_map>
#include <vector>

class Renderer;

struct PipelineKey
{
    Effect* effect;
    WGPUPrimitiveTopology topology;
    bool operator==(const PipelineKey&) const = default;
};

template<>
struct std::hash<PipelineKey>
{
    std::size_t operator()(const PipelineKey& k) const noexcept
    {
        auto h1 = std::hash<Effect*>{}(k.effect);
        auto h2 = std::hash<WGPUPrimitiveTopology>{}(k.topology);
        return h1 ^ (h2 << 1);
    }
};

/// A pipeline contains a WGPURenderPipeline, its layout, and a render queue.
class Pipeline
{
public:
    Pipeline(Renderer& renderer, const PipelineKey&);
    Pipeline(const Pipeline& other) = delete;
    Pipeline(Pipeline&&);

    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline& operator=(Pipeline&&);

    ~Pipeline();

    WGPURenderPipeline pipeline() { return m_pipeline; }

private:
    WGPUPipelineLayout m_pipeline_layout;
    WGPURenderPipeline m_pipeline;
};

/// A pipeline factory stores a pipeline cache.
class PipelineFactory
{
public:
    Pipeline& get_pipeline(Renderer& renderer, const PipelineKey&);

private:
    std::unordered_map<PipelineKey, Pipeline> m_pipelines;
};

#endif
