#include "addtile.h"
#include "../filedialog.h"
#include "noworry/resourcetable.h"
#include "imgui.h"

namespace fs = std::filesystem;

AddTile::AddTile(AppState& app_state)
    : Modal("Add Tile"),
      m_app_state(&app_state),
      m_preview(app_state.renderer().device(), 200, 200),
      m_selected_mesh(nullptr),
      m_scene(app_state.renderer(), m_camera),
      m_rotation(RotationTag::Rotate0),
      m_rotation_dropdown(m_rotation)
{
    m_preview.set_clear_color({1, 1, 1, 1});

    m_camera.set_position(glm::vec3(2.0f, -0.5f, 2.0f));
    m_camera.set_target(glm::vec3(2.0f, 0.5f, 1.0f));
}

ModalResponse AddTile::render()
{
    ModalResponse response = ModalResponse::KeepOpen;

    {
        Frame frame(m_app_state->renderer(), m_preview, m_scene);

        if (m_selected_mesh != nullptr) {
            frame.add_renderobject(m_selected_mesh->rotated0().renderobject());
        }
    }

    const char* label = m_selected_mesh == nullptr
        ? "Choose Mesh"
        : m_selected_mesh->name().c_str();

    if (ImGui::BeginCombo("Shape", label)) {
        for (auto& pair : m_app_state->mesh_map()) {
            if (ImGui::Selectable(pair.second->name().c_str(), false)) {
                m_selected_mesh = pair.second;
            }
        }
        ImGui::EndCombo();
    }

    m_rotation_dropdown.render();

    if (ImGui::Button("Choose Texture")) {
        m_app_state->modals().push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    if (m_sink.size() > 0) {
        m_texture = m_app_state->resources().load_texture(m_sink[0]);
        m_sink.clear();
    }

    if (m_texture.get() != nullptr) {
        ImGui::Image((ImTextureID)(intptr_t)m_texture->view(),
                     ImVec2(50, 50));
    }

    ImGui::Image((ImTextureID)(intptr_t)m_preview.texture().view(),
                 ImVec2(m_preview.width(), m_preview.height()));

    if (ImGui::Button("Add Tile")) {
        response = ModalResponse::Close;
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        response = ModalResponse::Close;
    }
    return response;
}
