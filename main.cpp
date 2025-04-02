#include "noworry/application.h"
#include "noworry/camera.h"
#include "noworry/mesh.h"
#include "noworry/meshbuilder.h"
#include "noworry/model.h"
#include "noworry/renderer.h"
#include "noworry/resourcetable.h"

#include <array>
#include <glm/ext/scalar_constants.hpp>
#include <webgpu/webgpu.h>

class Main : public Application
{
public:
    Main(int width, int height)
        : Application(width, height),
          m_renderer(device(), width, height),
          m_resources(m_renderer)
    {
        m_renderer.set_clear_color({0.5, 0.5, 0.5, 1});

        auto& mesh = MeshBuilder()
            .quad( // front
                XYZUV(-20, 20, -20, 0, 0),
                XYZUV(-20, -20, -20, 0, 1),
                XYZUV(20, -20, -20, 1, 1),
                XYZUV(20, 20, -20, 1, 0))
            .quad( // right
                XYZUV(20, 20, -20, 0, 0),
                XYZUV(20, -20, -20, 0, 1),
                XYZUV(20, -20, 20, 1, 1),
                XYZUV(20, 20, 20, 1, 0))
            .quad( // back
                XYZUV(20, 20, 20, 0, 0),
                XYZUV(20, -20, 20, 0, 1),
                XYZUV(-20, -20, 20, 1, 1),
                XYZUV(-20, 20, 20, 1, 0))
            .quad( // left
                XYZUV(-20, 20, 20, 0, 0),
                XYZUV(-20, -20, 20, 0, 1),
                XYZUV(-20, -20, -20, 1, 1),
                XYZUV(-20, 20, -20, 1, 0))
            .build(m_resources);

        auto& texture = m_resources.add_texture("../assets/cat.png");
        auto& material = m_resources.add_texture_material(texture);

        auto& material2 = m_resources.add_flat_material(1, 0, 0);

        m_yaw = glm::pi<float>() * 0.25f;
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
