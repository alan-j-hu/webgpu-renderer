#include "modelinspector.h"
#include "rotationdropdown.h"
#include "../filedialog.h"

#include "noworry/grid.h"
#include "imgui.h"

ModelInspector::ModelInspector(std::string name, int flex, AppState& app_state)
    : Pane(std::move(name), flex),
      m_app_state(&app_state),
      m_rotation(RotationTag::Rotate0),
      m_tile_preview(m_app_state->renderer().device(), 200, 200),
      m_scene(m_app_state->renderer(), m_camera)
{
    m_tile_preview.set_clear_color(app_state.background_color());

    m_camera.set_position(glm::vec3(2.0f, -0.5f, 2.0f));
    m_camera.set_target(glm::vec3(2.0f, 0.5f, 1.0f));
}

bool ModelInspector::render_preview()
{
    Transform transform;

    Frame frame(m_app_state->renderer(), m_tile_preview, m_scene);
    frame.add(transform,
              m_app_state->small_grid_mesh(),
              m_app_state->wireframe_material());
    if (m_selected_tile != nullptr) {
        frame.add(
            transform,
            m_selected_tile->rotated(m_rotation).mesh(),
            m_app_state->default_material());
    }
    return true;
}

void ModelInspector::content()
{
    namespace fs = std::filesystem;

    auto& mesh_map = m_app_state->mesh_map();
    if (mesh_map.size() == 0) {
        if (ImGui::Button("Choose Mesh File", ImVec2(0, 0))) {
            m_app_state->modals().push(
                std::make_unique<FileDialog>(fs::current_path(), m_sink));
        }
    } else {
        if (ImGui::BeginListBox("##Meshes", ImVec2(-FLT_MIN, 0))) {
            for (auto& pair : mesh_map) {
                bool selected = pair.second == m_selected_tile;
                if (ImGui::Selectable(pair.second->name().c_str(), selected)) {
                    m_selected_tile = pair.second;
                }
            }
            ImGui::EndListBox();
        }
    }

    rotation_dropdown(m_rotation);

    render_preview();
    ImGui::Image((ImTextureID)(intptr_t)m_tile_preview.texture().view(),
                 ImVec2(m_tile_preview.width(), m_tile_preview.height()));

    if (m_sink.size() != 0) {
        m_selected_tile = nullptr;
        m_app_state->load_meshes(m_sink[0]);
        m_sink.clear();
    }
}
