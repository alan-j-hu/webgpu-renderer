#include "tiledefinitioneditor.h"
#include "../filedialog.h"
#include "noworry/resourcetable.h"
#include "imgui.h"

namespace fs = std::filesystem;

TileDefinitionEditor::TileDefinitionEditor(
    AppState& app_state, TileDefinition& definition)
    : m_app_state(&app_state),
      m_definition(&definition),
      m_preview(app_state.renderer().device(), 200, 200),
      m_scene(app_state.renderer(), m_camera),
      m_transform(app_state.renderer()),
      m_rotation_dropdown(m_definition->rotation)
{
    m_preview.set_clear_color({1, 1, 1, 1});

    m_camera.set_position(glm::vec3(2.0f, -0.5f, 2.0f));
    m_camera.set_target(glm::vec3(2.0f, 0.5f, 1.0f));
}

void TileDefinitionEditor::render()
{
    draw_frame();

    const char* label = m_definition->selected_mesh == nullptr
        ? "Choose Mesh"
        : m_definition->selected_mesh->name().c_str();

    if (ImGui::BeginCombo("Shape", label)) {
        for (auto& pair : m_app_state->mesh_map()) {
            if (ImGui::Selectable(pair.second->name().c_str(), false)) {
                m_definition->selected_mesh = pair.second;
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
        m_definition->material =
            m_app_state->resources().load_texture_material(m_sink[0]);
        m_sink.clear();
    }

    if (m_definition->material.get() != nullptr) {
        ImGui::Image((ImTextureID)(intptr_t)
                     m_definition->material->texture().view(),
                     ImVec2(50, 50));
    }

    ImGui::Image((ImTextureID)(intptr_t)m_preview.texture().view(),
                 ImVec2(m_preview.width(), m_preview.height()));
}

Material& TileDefinitionEditor::get_material()
{
    if (m_definition->material.get() == nullptr) {
        return m_app_state->default_material();
    }
    return *m_definition->material;
}

void TileDefinitionEditor::draw_frame()
{
    Frame frame(m_app_state->renderer(), m_preview, m_scene);

    if (m_definition->selected_mesh != nullptr) {
        switch (m_definition->rotation) {
        case RotationTag::Rotate0:
            frame.add(m_transform,
                      m_definition->selected_mesh->rotated0().mesh(),
                      get_material());
            break;
        case RotationTag::Rotate90:
            frame.add(m_transform,
                      m_definition->selected_mesh->rotated90().mesh(),
                      get_material());
            break;
        case RotationTag::Rotate180:
            frame.add(m_transform,
                      m_definition->selected_mesh->rotated180().mesh(),
                      get_material());
            break;
        default:
            frame.add(m_transform,
                      m_definition->selected_mesh->rotated270().mesh(),
                      get_material());
            break;
        }
    }
}
