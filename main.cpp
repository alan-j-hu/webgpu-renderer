#include "application.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "pipeline.h"
#include "renderer.h"

#include <array>
#include <webgpu/webgpu.h>

class Main : public Application
{
public:
    Main(int width, int height)
        : Application(width, height),
          m_renderer(device(), width, height)
    {
        std::array<Vertex, 3> vertices;
        vertices[0].x = 0;
        vertices[0].y = 0;
        vertices[0].z = 50;
        vertices[0]._padding = 0;
        vertices[0].r = 1;
        vertices[0].g = 0;
        vertices[0].b = 0;
        vertices[0].a = 1;

        vertices[1].x = 20;
        vertices[1].y = 20;
        vertices[1].z = 50;
        vertices[1]._padding = 0;
        vertices[1].r = 0;
        vertices[1].g = 1;
        vertices[1].b = 0;
        vertices[1].a = 1;

        vertices[2].x = 0;
        vertices[2].y = 20;
        vertices[2].z = 50;
        vertices[2]._padding = 0;
        vertices[2].r = 0;
        vertices[2].g = 0;
        vertices[2].b = 1;
        vertices[2].a = 1;
        auto& mesh = m_renderer.add_mesh(vertices.data(), 3);
        m_renderer.add_model(mesh);
    }

    virtual void render(WGPUTextureView view) override
    {
        m_renderer.render(view);
    }

private:
    Renderer m_renderer;
};

int main()
{
    Main app(500, 500);
    app.run();

    return 0;
}
