#include "noworry/application.h"
#include "noworry/camera.h"
#include "noworry/mesh.h"
#include "noworry/model.h"
#include "noworry/renderer.h"
#include "noworry/resourcetable.h"

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

void make_quad(
    Vertex* vs,
    std::uint16_t* is,
    int v_off,
    int i_off,
    Vertex a, Vertex b, Vertex c, Vertex d)
{
    vs[v_off] = a;
    vs[v_off + 1] = b;
    vs[v_off + 2] = c;
    vs[v_off + 3] = d;

    is[i_off] = v_off;
    is[i_off + 1] = v_off + 1;
    is[i_off + 2] = v_off + 2;
    is[i_off + 3] = v_off;
    is[i_off + 4] = v_off + 2;
    is[i_off + 5] = v_off + 3;
}

class Main : public Application
{
public:
    Main(int width, int height)
        : Application(width, height),
          m_renderer(device(), width, height),
          m_resources(m_renderer)
    {
        m_renderer.set_clear_color({0.5, 0.5, 0.5, 1});

        std::array<Vertex, 16> vertices;
        std::array<std::uint16_t, 24> indices;
        // front
        make_quad(
            vertices.data(),
            indices.data(),
            0,
            0,
            make_vertex(-20, 20, -20, 0, 0),
            make_vertex(-20, -20, -20, 1, 0),
            make_vertex(20, -20, -20, 1, 1),
            make_vertex(20, 20, -20, 0, 1));
        // right
        make_quad(
            vertices.data(),
            indices.data(),
            4,
            6,
            make_vertex(20, 20, -20, 0, 0),
            make_vertex(20, -20, -20, 1, 0),
            make_vertex(20, -20, 20, 1, 1),
            make_vertex(20, 20, 20, 0, 1));
        // back
        make_quad(
            vertices.data(),
            indices.data(),
            8,
            12,
            make_vertex(20, 20, 20, 0, 0),
            make_vertex(20, -20, 20, 1, 0),
            make_vertex(-20, -20, 20, 1, 1),
            make_vertex(-20, 20, 20, 0, 1));
        // left
        make_quad(
            vertices.data(),
            indices.data(),
            12,
            18,
            make_vertex(-20, 20, 20, 0, 0),
            make_vertex(-20, -20, 20, 1, 0),
            make_vertex(-20, -20, -20, 1, 1),
            make_vertex(-20, 20, -20, 0, 1));

        auto& texture = m_resources.add_texture("../assets/cat.png");
        auto& material = m_resources.add_texture_material(texture);

        m_yaw = glm::pi<float>() * 0.25f;
        auto& mesh = m_resources.add_mesh(
            vertices.data(), 16, indices.data(), 8);
        m_model = &m_resources.add_model(mesh, material);
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
    ResourceTable m_resources;
    Model* m_model;
    float m_yaw;
};

int main()
{
    Main app(500, 500);
    app.run();

    return 0;
}
