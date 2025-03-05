#include "application.h"
#include "camera.h"
#include "model.h"
#include "pipeline.h"

#include <array>
#include <iostream>
#include <webgpu/webgpu.h>

class Main : public Application
{
public:
    Main(int width, int height)
        : Application(width, height),
          m_pipeline(device()),
          m_camera(device(), m_pipeline),
          m_model(device(), m_pipeline)
    {
        std::array<Vertex, 3> vertices;
        vertices[0].x = 0;
        vertices[0].y = 0;
        vertices[0].z = 0;
        vertices[0]._padding = 0;
        vertices[0].r = 1;
        vertices[0].g = 0;
        vertices[0].b = 0;
        vertices[0].a = 1;

        vertices[1].x = 1;
        vertices[1].y = 1;
        vertices[1].z = 0;
        vertices[1]._padding = 0;
        vertices[1].r = 0;
        vertices[1].g = 1;
        vertices[1].b = 0;
        vertices[1].a = 1;

        vertices[2].x = 0;
        vertices[2].y = 1;
        vertices[2].z = 0;
        vertices[2]._padding = 0;
        vertices[2].r = 0;
        vertices[2].g = 0;
        vertices[2].b = 1;
        vertices[2].a = 1;
        create_vertices(vertices.data(), 3);
    }

    void create_vertices(Vertex* vertices, std::size_t count)
    {
        WGPUBufferDescriptor buffer_desc = { 0 };
        buffer_desc.nextInChain = nullptr;
        buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex,
        buffer_desc.size = count * sizeof(Vertex);
        buffer_desc.mappedAtCreation = false;

        m_vertex_buffer =
            wgpuDeviceCreateBuffer(device(), &buffer_desc);

        WGPUQueue queue = wgpuDeviceGetQueue(device());
        wgpuQueueWriteBuffer(
            queue, m_vertex_buffer, 0, vertices, sizeof(Vertex) * count);
    }


    virtual void pre_render(WGPUQueue queue)
    {
        m_camera.copy_to_gpu(device());
        m_model.copy_to_gpu(device());
    }

    virtual void render(WGPURenderPassEncoder encoder) override
    {
        wgpuRenderPassEncoderSetBindGroup(encoder, 0, m_camera.m_bind_group, 0, nullptr);
        wgpuRenderPassEncoderSetBindGroup(encoder, 1, m_model.m_bind_group, 0, nullptr);
        wgpuRenderPassEncoderSetPipeline(encoder, m_pipeline.pipeline());
        wgpuRenderPassEncoderSetVertexBuffer(encoder, 0, m_vertex_buffer, 0, sizeof(Vertex) * 3);
        wgpuRenderPassEncoderDraw(encoder, 3, 1, 0, 0);
    }

    virtual ~Main()
    {
        wgpuBufferRelease(m_vertex_buffer);
    }
private:
    Pipeline m_pipeline;
    Camera m_camera;
    Model m_model;
    WGPUBuffer m_vertex_buffer;
};

int main()
{
    Main app(500, 500);
    app.run();

    return 0;
}
