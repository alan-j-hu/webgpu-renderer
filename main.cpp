#include "src/application.h"
#include "src/camera.h"
#include "src/mesh.h"
#include "src/model.h"
#include "src/pipeline.h"
#include "src/renderer.h"

#include <array>
#include <glm/ext/scalar_constants.hpp>
#include <webgpu/webgpu.h>

Vertex make_vertex(
    float x, float y, float z, float u, float v)
{
    Vertex vertex = { 0 };
    vertex.x = x;
    vertex.y = y;
    vertex.z = z;
    vertex.u = u;
    vertex.v = v;
    return vertex;
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
            make_vertex(-20, 20, -20, 0, 0),
            make_vertex(-20, -20, -20, 1, 0),
            make_vertex(20, -20, -20, 1, 1),
            make_vertex(20, 20, -20, 0, 1));
        // right
        make_quad(
            vertices.data() + 6,
            make_vertex(20, 20, -20, 0, 0),
            make_vertex(20, -20, -20, 1, 0),
            make_vertex(20, -20, 20, 1, 1),
            make_vertex(20, 20, 20, 0, 1));
        // back
        make_quad(
            vertices.data() + 12,
            make_vertex(20, 20, 20, 0, 0),
            make_vertex(20, -20, 20, 1, 0),
            make_vertex(-20, -20, 20, 1, 1),
            make_vertex(-20, 20, 20, 0, 1));
        // left
        make_quad(
            vertices.data() + 18,
            make_vertex(-20, 20, 20, 0, 0),
            make_vertex(-20, -20, 20, 1, 0),
            make_vertex(-20, -20, -20, 1, 1),
            make_vertex(-20, 20, -20, 0, 1));

        auto& texture = m_renderer.add_texture("../assets/cat.png");
        auto& material = m_renderer.add_material(texture);

        m_yaw = glm::pi<float>() * 0.25f;
        auto& mesh = m_renderer.add_mesh(vertices.data(), 24, material);
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

    virtual void resize(int width, int height) override
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
