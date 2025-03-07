#include "application.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "pipeline.h"
#include "renderer.h"

#include <array>
#include <glm/ext/scalar_constants.hpp>
#include <webgpu/webgpu.h>

Vertex make_vertex(
    float x, float y, float z, float r, float g, float b, float a)
{
    Vertex v = { 0 };
    v.x = x;
    v.y = y;
    v.z = z;
    v.r = r;
    v.g = g;
    v.b = b;
    v.a = a;
    return v;
}

void make_quad(Vertex* out, Vertex a, Vertex b, Vertex c, Vertex d)
{
    out[0] = a;
    out[1] = b;
    out[2] = c;
    out[3] = a;
    out[4] = c;
    out[5] = d;
}

class Main : public Application
{
public:
    Main(int width, int height)
        : Application(width, height),
          m_renderer(device(), width, height)
    {
        m_renderer.set_clear_color({0.5, 0.5, 0.5, 1});

        std::array<Vertex, 24> vertices;
        // front
        make_quad(
            vertices.data(),
            make_vertex(-20, 20, -20, 1, 0, 0, 1),
            make_vertex(-20, -20, -20, 0, 1, 0, 1),
            make_vertex(20, -20, -20, 0, 0, 1, 1),
            make_vertex(20, 20, -20, 1, 0, 0, 1));
        // right
        make_quad(
            vertices.data() + 6,
            make_vertex(20, 20, -20, 1, 0, 0, 1),
            make_vertex(20, -20, -20, 0, 1, 0, 1),
            make_vertex(20, -20, 20, 0, 0, 1, 1),
            make_vertex(20, 20, 20, 1, 0, 0, 1));
        // back
        make_quad(
            vertices.data() + 12,
            make_vertex(20, 20, 20, 1, 0, 0, 1),
            make_vertex(20, -20, 20, 0, 1, 0, 1),
            make_vertex(-20, -20, 20, 0, 0, 1, 1),
            make_vertex(-20, 20, 20, 1, 0, 0, 1));
        // left
        make_quad(
            vertices.data() + 18,
            make_vertex(-20, 20, 20, 1, 0, 0, 1),
            make_vertex(-20, -20, 20, 0, 1, 0, 1),
            make_vertex(-20, -20, -20, 0, 0, 1, 1),
            make_vertex(-20, 20, -20, 1, 0, 0, 1));

        m_yaw = glm::pi<float>() * 0.25f;
        auto& mesh = m_renderer.add_mesh(vertices.data(), 24);
        m_model = &m_renderer.add_model(mesh);
        m_model->set_translation(glm::vec3(0.0f, 0.0f, 50.0f));

        auto& camera = m_renderer.camera();
        camera.set_position(glm::vec3(0.0f, 0.0f, -25.0f));
        camera.set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    virtual void render(WGPUTextureView view) override
    {
        m_yaw += 0.01;
        m_model->set_yaw(m_yaw);
        m_renderer.render(view);
    }

    virtual void resize(int width, int height)
    {
        Application::resize(width, height);
        m_renderer.resize(width, height);
    }

private:
    Renderer m_renderer;
    Model* m_model;
    float m_yaw;
};

int main()
{
    Main app(500, 500);
    app.run();

    return 0;
}
