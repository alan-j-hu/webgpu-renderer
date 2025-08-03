#ifndef RENDER_BATCH_H
#define RENDER_BATCH_H

#include "../Pipeline/PipelineFactory.h"
#include "../scene/renderobject.h"
#include <webgpu/webgpu.h>
#include <unordered_map>
#include <vector>

class RenderBatch
{
public:
    void set_pipeline(Pipeline& pipeline);

    void enqueue(RenderObject&);

    void draw(Renderer&, WGPURenderPassEncoder encoder);

private:
    Pipeline* m_pipeline;
    std::vector<RenderObject*> m_draw_calls;
};

class RenderBatcher
{
public:
    RenderBatcher(Renderer&);

    void enqueue(RenderObject&);

    void draw(WGPURenderPassEncoder encoder);

private:
    Renderer* m_renderer;

    std::unordered_map<PipelineKey, RenderBatch*> m_batches;
    std::vector<RenderBatch> m_pool;
    int m_pool_index;

    RenderBatch* next_free(Pipeline&);

    void reset();

    RenderBatch* search(RenderObject&);
};

#endif
