#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "Frame2D.h"
#include "SpritePipeline.h"
#include "Spritesheet.h"

#include <cstddef>
#include <optional>
#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <webgpu/webgpu.h>

struct Region
{
    float x;
    float y;
    float w;
    float h;
};

// TODO: Investigate more efficient sprite renderer implementation
class SpriteRenderer
{
public:
    SpriteRenderer(WGPUDevice device, std::size_t max);

    virtual ~SpriteRenderer();

    SpritePipeline& pipeline() { return m_pipeline; }

    void resize(int size);

    void begin(RenderTarget&);

    void end();

    void draw(
        const Spritesheet& spritesheet,
        glm::vec2 pos,
        float width,
        float height,
        const Region& src = { 0.0, 0.0, 1.0, 1.0 },
        float rotation = 0.0,
        glm::vec2 origin = glm::vec2(0.0, 0.0));

private:
    std::size_t m_capacity;
    WGPUDevice m_device;
    SpritePipeline m_pipeline;

    std::optional<Frame2D> m_frame;

    struct DrawCall
    {
        WGPUBindGroup bind_group;
        int begin;
        int end;
    } m_current_draw_call;

    std::vector<DrawCall> m_draw_calls;
    std::vector<glm::vec4> m_vertices;
    WGPUBuffer m_vertex_buffer;
    WGPUBuffer m_index_buffer;

    WGPUBuffer m_viewproj_buffer;
    WGPUBindGroup m_global_bind_group;

    void flush();
};

#endif
