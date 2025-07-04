#ifndef SPRITEBATCH_H
#define SPRITEBATCH_H

#include "SpritePipeline.h"
#include "Spritesheet.h"

#include <cstddef>
#include <vector>

#include <glm/vec4.hpp>
#include <webgpu/webgpu.h>

struct Region
{
    int x;
    int y;
    int w;
    int h;
};

class SpriteBatch
{
public:
    SpriteBatch(WGPUDevice device, std::size_t max);

    virtual ~SpriteBatch();

    void begin(WGPURenderPassEncoder encoder);

    void end();

    void draw(const Spritesheet&, const Region& dest, const Region& src);

private:
    std::size_t m_capacity;
    WGPUDevice m_device;
    SpritePipeline m_pipeline;

    WGPURenderPassEncoder m_encoder;
    bool m_is_drawing;

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

    void flush();
};

#endif
