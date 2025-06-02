#include "modelinspector.h"
#include "../filedialog.h"

#include "noworry/grid.h"
#include "imgui.h"

ModelInspector::ModelInspector(std::string name, int flex, AppState& app_state)
    : Pane(std::move(name), flex),
      m_app_state(app_state),
      m_rotation(RotationTag::Rotate0),
      m_rotation_dropdown(m_rotation),
      m_tile_preview(m_app_state.renderer().device(), 200, 200),
      m_scene(m_app_state.renderer(), m_camera),
      m_grid_mesh(
          create_grid(m_app_state.renderer().device(),
                      glm::vec3(0, 0, 0),
                      5,
                      5,
                      glm::vec3(5, 0, 0),
                      glm::vec3(0, 5, 0)))
{
    m_tile_preview.set_clear_color({1, 1, 1, 1});

    m_grid = std::make_unique<RenderObject>(
        m_app_state.renderer().device(),
        m_grid_mesh,
        m_app_state.wireframe_material());

    m_camera.set_position(glm::vec3(2.0f, -0.5f, 2.0f));
    m_camera.set_target(glm::vec3(2.0f, 0.5f, 1.0f));
}

bool ModelInspector::render_preview()
{
    Frame frame(m_app_state.renderer(), m_tile_preview, m_scene);
    frame.add_renderobject(*m_grid);
    if (m_selected_tile != nullptr) {
        switch (m_rotation) {
        case RotationTag::Rotate0: {
            frame.add_renderobject(
                m_selected_tile->rotated0().renderobject());
        }
        break;
        case RotationTag::Rotate90: {
            frame.add_renderobject(
                m_selected_tile->rotated90().renderobject());
        }
        break;
        case RotationTag::Rotate180: {
            frame.add_renderobject(
                m_selected_tile->rotated180().renderobject());
        }
        break;
        case RotationTag::Rotate270: {
            frame.add_renderobject(
                m_selected_tile->rotated270().renderobject());
        }
        break;
        }
    }
    return true;
}

void ModelInspector::content()
{
    namespace fs = std::filesystem;

    if (ImGui::Button("Choose Mesh File", ImVec2(0, 0))) {
        m_app_state.modals().push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    if (ImGui::BeginListBox("##Meshes", ImVec2(-FLT_MIN, 0))) {
        for (auto& pair : m_app_state.mesh_map()) {
            bool selected = pair.second == m_selected_tile;
            if (ImGui::Selectable(pair.second->name().c_str(), selected, 0)) {
                m_selected_tile = pair.second;
            }
        }
        ImGui::EndListBox();
    }

    m_rotation_dropdown.render();

    render_preview();
    ImGui::Image((ImTextureID)(intptr_t)m_tile_preview.texture().view(),
                 ImVec2(m_tile_preview.width(), m_tile_preview.height()));

    if (m_sink.size() != 0) {
        m_selected_tile = nullptr;
        m_app_state.load_meshes(m_sink[0]);
        m_sink.clear();
    }
}
