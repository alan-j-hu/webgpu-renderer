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
      m_scene(m_app_state->renderer(), m_camera),
      m_transform(m_app_state->renderer()),
      m_grid_mesh(
          create_grid(m_app_state->renderer().device(),
                      glm::vec3(0, 0, 0),
                      5,
                      5,
                      glm::vec3(5, 0, 0),
                      glm::vec3(0, 5, 0)))
{
    m_tile_preview.set_clear_color(app_state.background_color());

    m_camera.set_position(glm::vec3(2.0f, -0.5f, 2.0f));
    m_camera.set_target(glm::vec3(2.0f, 0.5f, 1.0f));
}

bool ModelInspector::render_preview()
{
    Frame frame(m_app_state->renderer(), m_tile_preview, m_scene);
    frame.add(m_transform, m_grid_mesh, m_app_state->wireframe_material());
    if (m_selected_tile != nullptr) {
        switch (m_rotation) {
        case RotationTag::Rotate0: {
            frame.add(
                m_transform,
                m_selected_tile->rotated0().mesh(),
                m_app_state->default_material());
        }
        break;
        case RotationTag::Rotate90: {
            frame.add(
                m_transform,
                m_selected_tile->rotated90().mesh(),
                m_app_state->default_material());
        }
        break;
        case RotationTag::Rotate180: {
            frame.add(
                m_transform,
                m_selected_tile->rotated180().mesh(),
                m_app_state->default_material());
        }
        break;
        case RotationTag::Rotate270: {
            frame.add(
                m_transform,
                m_selected_tile->rotated270().mesh(),
                m_app_state->default_material());
        }
        break;
        }
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
