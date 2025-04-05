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

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_wgpu.h"

class Main : public Application
{
public:
    Main(int width, int height)
        : Application(width, height, WGPUTextureFormat_BGRA8Unorm),
          m_subwindow(device(), 500, 500,
                      WGPUTextureFormat_BGRA8Unorm,
                      WGPUTextureUsage_TextureBinding
                      | WGPUTextureUsage_RenderAttachment),
          m_renderer(device(), m_subwindow.width(), m_subwindow.height()),
          m_resources(m_renderer)
    {
        init_imgui();
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

    virtual ~Main()
    {
        shutdown_imgui();
    }

    virtual void render(WGPUTextureView view) override
    {
        draw_imgui();

        WGPURenderPassColorAttachment color_attachments = {};
        color_attachments.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
        color_attachments.loadOp = WGPULoadOp_Clear;
        color_attachments.storeOp = WGPUStoreOp_Store;
        color_attachments.clearValue = {0, 0, 0, 1};
        color_attachments.view = view;

        WGPURenderPassDescriptor render_pass_desc = {};
        render_pass_desc.colorAttachmentCount = 1;
        render_pass_desc.colorAttachments = &color_attachments;
        render_pass_desc.depthStencilAttachment = nullptr;

        WGPUCommandEncoderDescriptor enc_desc = {};
        WGPUCommandEncoder encoder =
            wgpuDeviceCreateCommandEncoder(device(), &enc_desc);

        WGPURenderPassEncoder pass =
            wgpuCommandEncoderBeginRenderPass(encoder, &render_pass_desc);
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass);
        wgpuRenderPassEncoderEnd(pass);

        WGPUCommandBufferDescriptor cmd_buffer_desc = {};
        WGPUCommandBuffer cmd_buffer =
            wgpuCommandEncoderFinish(encoder, &cmd_buffer_desc);
        WGPUQueue queue = wgpuDeviceGetQueue(device());
        wgpuQueueSubmit(queue, 1, &cmd_buffer);
    }

    virtual void resize(int width, int height) override
    {
        ImGui_ImplWGPU_InvalidateDeviceObjects();
        Application::resize(width, height);
        ImGui_ImplWGPU_CreateDeviceObjects();
    }

private:
    const ImGuiWindowFlags WINDOW_FLAGS =
        ImGuiWindowFlags_NoMove
      | ImGuiWindowFlags_MenuBar
      | ImGuiWindowFlags_NoDecoration;

    Texture m_subwindow;

    Renderer m_renderer;
    ResourceTable m_resources;
    Model* m_model;
    float m_yaw;

    void init_imgui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOther(window(), true);
        ImGui_ImplWGPU_InitInfo init_info;
        init_info.Device = device();
        init_info.NumFramesInFlight = 3;
        init_info.RenderTargetFormat = WGPUTextureFormat_BGRA8Unorm;
        init_info.DepthStencilFormat = WGPUTextureFormat_Undefined;
        ImGui_ImplWGPU_Init(&init_info);
    }

    void shutdown_imgui()
    {
        ImGui_ImplWGPU_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void draw_imgui()
    {
        render_subwindow();

        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::Begin("Window", nullptr, WINDOW_FLAGS);

        draw_menubar();

        ImGui::Image((ImTextureID)(intptr_t)m_subwindow.view(),
                     ImVec2(m_subwindow.width(), m_subwindow.height()));
        ImGui::End();

        ImGui::Render();
    }

    bool draw_menubar()
    {
        if (!ImGui::BeginMenuBar()) return false;
        ImGui::MenuItem("File");
        ImGui::MenuItem("Edit");
        ImGui::EndMenuBar();
        return true;
    };

    void render_subwindow()
    {
        m_yaw += 0.01;
        m_model->set_yaw(m_yaw);
        m_renderer.render(m_subwindow.view());
    }
};

int main()
{
    Main app(1000, 1000);
    app.run();

    return 0;
}
