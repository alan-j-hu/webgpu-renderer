#ifndef RENDER_BATCH_H
#define RENDER_BATCH_H

#include "../Pipeline/PipelineFactory.h"
#include "../scene/renderobject.h"
#include <webgpu/webgpu.h>
#include <memory>
#include <unordered_map>
#include <vector>

#include <glm/mat4x4.hpp>

struct DrawCall
{
    const Mesh* mesh;
    WGPUBindGroup material_group;
    glm::mat4 transform;

    DrawCall(const Mesh& p_mesh,
             const Material& p_material,
             const glm::mat4& p_transform);
};

class RenderBatch
{
public:
    void set_pipeline(Pipeline& pipeline);

    void enqueue(RenderObject&);

    void enqueue_parts(const Mesh& mesh,
                       const Material& mat,
                       const glm::mat4& transform);

    void draw(Renderer&, WGPURenderPassEncoder encoder);

private:
    Pipeline* m_pipeline;
    std::vector<DrawCall> m_draw_calls;
};

class RenderBatcher
{
public:
    RenderBatcher(Renderer&);

    void enqueue(RenderObject&);

    void enqueue_parts(const Mesh& mesh,
                       const Material& mat,
                       const glm::mat4& transform);

    void draw(WGPURenderPassEncoder encoder);

private:
    Renderer* m_renderer;

    std::unordered_map<PipelineKey, RenderBatch*> m_batches;
    std::vector<std::unique_ptr<RenderBatch>> m_pool;
    int m_pool_index;

    RenderBatch* next_free(Pipeline&);

    void reset();

    RenderBatch* search(const Material& material, const Mesh& mesh);
};

#endif
