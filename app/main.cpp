#include "AppState.h"
#include "Tilemap/TilemapEditor.h"
#include "Tileset/TilesetPane.h"

#include "noworry/application.h"
#include "noworry/mesh.h"
#include "noworry/renderer.h"
#include "noworry/rendertarget.h"
#include "noworry/resourcetable.h"
#include "noworry/scene/renderobject.h"
#include "noworry/scene/scene.h"

#include <array>
#include <filesystem>
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
          m_subwindow(device(), 500, 500),
          m_app_state(device()),
          m_tileset_editor(1, m_app_state),
          m_tilemap_editor(m_app_state)
    {
        init_imgui();

        m_subwindow.set_clear_color({0.5, 0.5, 0.5, 1});
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
      | ImGuiWindowFlags_NoBringToFrontOnFocus
      | ImGuiWindowFlags_NoDecoration;

    RenderTarget m_subwindow;

    AppState m_app_state;

    TilesetPane m_tileset_editor;
    TilemapEditor m_tilemap_editor;

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
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::BeginDisabled(m_app_state.modals().has_focus());

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::Begin("Window", nullptr, WINDOW_FLAGS);

        m_app_state.refresh_thumbnails();

        draw_menubar();

        draw_main_pane();

        ImGui::End();

        ImGui::EndDisabled();

        m_app_state.modals().render();

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

    void draw_main_pane()
    {
        if (ImGui::BeginTabBar("Tabs")) {
            if (ImGui::BeginTabItem("Tilemap Editor")) {
                m_tilemap_editor.render();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Tileset Editor")) {
                m_tileset_editor.resize(500, 500);
                m_tileset_editor.render();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
          }
    }
};

int main()
{
    Main app(1000, 1000);
    app.run();

    return 0;
}
