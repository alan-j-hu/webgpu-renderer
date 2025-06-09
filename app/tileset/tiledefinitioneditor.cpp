#include "tiledefinitioneditor.h"
#include "rotationdropdown.h"
#include "../filedialog.h"
#include "noworry/resourcetable.h"
#include "imgui.h"

namespace fs = std::filesystem;

TileDefinitionEditor::TileDefinitionEditor(AppState& app_state)
    : m_app_state(&app_state),
      m_preview(app_state.renderer().device(), 200, 200),
      m_scene(app_state.renderer(), m_camera)
{
    m_preview.set_clear_color(app_state.background_color());

    m_camera.set_position(glm::vec3(2.0f, -0.5f, 2.0f));
    m_camera.set_target(glm::vec3(2.0f, 0.5f, 1.0f));
}

void TileDefinitionEditor::render(TileDefinition& definition)
{
    draw_frame(definition);

    const char* label = definition.selected_mesh == nullptr
        ? "Choose Mesh"
        : definition.selected_mesh->name().c_str();

    if (ImGui::BeginCombo("Shape", label)) {
        for (auto& pair : m_app_state->mesh_map()) {
            if (ImGui::Selectable(pair.second->name().c_str(), false)) {
                definition.selected_mesh = pair.second;
            }
        }
        ImGui::EndCombo();
    }

    rotation_dropdown(definition.rotation);

    if (ImGui::Button("Choose Texture")) {
        m_app_state->modals().push(
            std::make_unique<FileDialog>(fs::current_path(), m_sink));
    }

    if (m_sink.size() > 0) {
        definition.material =
            m_app_state->resources().load_texture_material(m_sink[0]);
        m_sink.clear();
    }

    if (definition.material.get() != nullptr) {
        ImGui::Image((ImTextureID)(intptr_t)
                     definition.material->texture().view(),
                     ImVec2(50, 50));
    }

    ImGui::Image((ImTextureID)(intptr_t)m_preview.texture().view(),
                 ImVec2(m_preview.width(), m_preview.height()));
}

Material& TileDefinitionEditor::get_material(TileDefinition& definition)
{
    if (definition.material.get() == nullptr) {
        return m_app_state->default_material();
    }
    return *definition.material;
}

void TileDefinitionEditor::draw_frame(TileDefinition& definition)
{
    Frame frame(m_app_state->renderer(), m_preview, m_scene);

    if (definition.selected_mesh != nullptr) {
        frame.add(m_transform,
                  definition.tile_mesh().mesh(),
                  get_material(definition));
    }
}
